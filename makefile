main:
	find tcp -name makefile | sort | xargs -n1 make -f
	
nowarn:
	find tcp -name makefile | sort | xargs -n1 make nowarn -f
	
clean:
	find tcp -name makefile | sort | xargs -n1 make clean -f
