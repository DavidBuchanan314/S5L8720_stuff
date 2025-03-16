

all: payload

.PHONY: payload clean pwn

payload:
	make -C payload

pwn: payload
	sudo python3 launch_payload.py

clean:
	make -C payload clean