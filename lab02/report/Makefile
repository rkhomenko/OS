REPORT_BUILD_DIR=$(BUILD_DIR)/lab02/report
JOBNAME=lab02_report

$(REPORT_BUILD_DIR)/$(JOBNAME).pdf: dirs report.tex
	latexmk -silent -pdf -jobname=$(JOBNAME) -outdir=$(REPORT_BUILD_DIR)

dirs:
	@mkdir -p $(REPORT_BUILD_DIR)

clean:
	latexmk -silent -CA -jobname=$(JOBNAME) -outdir=$(REPORT_BUILD_DIR)

.PHONY: clean