from elftools.elf.elffile import ELFFile
from functools import reduce
import operator

class Mapper():
	def __init__(self, base=0, align=0x1000):
		self.mem = bytearray()
		self.flags = []
		self.align = align
		self.base = base
	
	def map(self, start, data, flags):
		start -= self.base

		# pre-extend the storage arrays, if necessary
		if start + len(data) > len(self.mem):
			extra_needed = start + len(data) - len(self.mem)
			self.mem += bytearray(extra_needed)
			self.flags += [0] * extra_needed
		
		# map the data
		self.mem[start:start+len(data)] = data
		self.flags[start:start+len(data)] = [flags] * len(data)

		# pad up to page boundary
		pad_len = -len(self.mem) % self.align
		self.mem += bytearray(pad_len)
		self.flags += [0] * (pad_len)
	
	def get_page_maps(self): # returns iterator of (start, length, perms)
		map_start = 0
		prev_flags = None

		for page_start in range(0, len(self.mem), self.align):
			page_flags = reduce(
				operator.or_,
				self.flags[page_start:page_start+self.align]
			)

			if prev_flags is not None and page_flags != prev_flags:
				map_len = page_start - map_start
				yield (map_start + self.base, map_len, prev_flags)
				map_start = page_start
			
			prev_flags = page_flags
		
		if prev_flags is not None:
			yield (map_start + self.base, len(self.mem) - map_start, prev_flags)


def flags_to_string(flags):
	return ('r' if flags & 4 else '-') \
		+ ('w' if flags & 2 else '-') \
		+ ('x' if flags & 1 else '-')


def elf2bin(elf_file, verbose=True):
	elf = ELFFile(elf_file)

	image_base = min(seg.header.p_vaddr for seg in elf.iter_segments() if seg.header.p_type == "PT_LOAD")

	assert(image_base == 0x22001000)

	if verbose:
		print("[+] ELF Image base: ", hex(image_base))
		print("[+] ELF Entry: ", hex(elf.header.e_entry))
		print()
		print("[+] ELF segments:")
		print("\tType             Offset     VAddr      FileSize   MemSize    Prot")
	
	mapper = Mapper(base=image_base)

	for seg in elf.iter_segments():
		if verbose:
			print(f"\t{seg.header.p_type:<16} "\
				f"0x{seg.header.p_offset:08x} "\
				f"0x{seg.header.p_vaddr:08x} "\
				f"0x{seg.header.p_filesz:08x} "\
				f"0x{seg.header.p_memsz:08x} "\
				f"{flags_to_string(seg.header.p_flags)}")

		# We only actually care about PT_LOAD segments
		if seg.header.p_type != "PT_LOAD":
			continue

		padded_data = seg.data() + bytes(seg.header.p_memsz - seg.header.p_filesz)
		mapper.map(seg.header.p_vaddr, padded_data, seg.header.p_flags)

	if verbose:
		print()
		print("[+] Page mappings:")
		print("\tStart      Length     Prot")
	
		for start, length, flags in mapper.get_page_maps():
			print(f"\t0x{start:08x} 0x{length:08x} {flags_to_string(flags)}")

	# ldr r0, =entry; bx r0
	trampoline = b"\x00\x48\x00\x47" + (elf.header.e_entry | 1).to_bytes(4, "little")
	trampoline += bytes(0x1000 - len(trampoline))

	image = trampoline + mapper.mem

	return image


if __name__ == "__main__":
	import sys

	if len(sys.argv) != 3:
		print(f"USAGE: python3 {sys.argv[0]} input.elf output.bin")
		exit()
	
	elf_file = open(sys.argv[1], "rb")
	image = elf2bin(elf_file)
	with open(sys.argv[2], "wb") as bin_file:
		bin_file.write(image)