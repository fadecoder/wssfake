fake:wss_fake.c
	 gcc -O2 -o wss_fake wss_fake.c
s:wss_fake_stage.c
	 gcc -O2 -o wss_fake wss_fake_stage.c
	 cp wss_fake /wss_fake -f
n:wss_fake_normal.c
	 gcc -O2 -o wss_fake wss_fake_normal.c
	 cp wss_fake /wss_fake -f
clean:
	rm wss_fake -f
