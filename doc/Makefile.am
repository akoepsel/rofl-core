
.PHONY: doc

if have_doxygen 

doc: 
	@echo -n Generating documentation of ROFL-common \(this may take a while\)...
	@ SRC_DIR=$(top_srcdir) @DOXYGEN@ doxyfile-common.conf >/dev/null 2>&1
	@echo done
	
	@echo -n Generating documentation of ROFL-pipeline \(this may take a while\)...
	@ SRC_DIR=$(top_srcdir) @DOXYGEN@ doxyfile-pipeline.conf >/dev/null 2>&1
	@echo done
	
	@echo -n Generating documentation of ROFL-hal \(this may take a while\)...
	@ SRC_DIR=$(top_srcdir) @DOXYGEN@ doxyfile-hal.conf >/dev/null 2>&1
	@echo done
	
	@echo  The documentation is under: $(builddir)/doc/ 
else

doc: 
	@echo "ERROR: unable to generate documentation; DOXYGEN not found!" 
	@exit -1

endif
