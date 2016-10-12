fake:wss_fake.c
	 gcc -O2 -o wss_fake wss_fake.c
s:wss_fake_stage.c
	 gcc -O2 -o wss_fake wss_fake_stage.c
n:wss_fake_normal.c
	 gcc -O2 -o wss_fake wss_fake_normal.c
m:wss_fake_normal.c
	 gcc -o wss_fake wss_fake_normal.c
r:wss_fake_repeat.c
	 gcc -o wss_fake wss_fake_repeat.c
re:wss_fake_read.c
	 gcc -o wss_fake wss_fake_read.c
clean:
	rm wss_fake -f
