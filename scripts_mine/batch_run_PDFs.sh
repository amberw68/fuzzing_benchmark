while getopts h:p:o: option
do
        case "${option}"
        in
        t) TOOL=${OPTARG};; # Tool binary you want to test
	c) COMMAND=${OPTARG};; # Tool needed commands or options
        p) PDF_DIR=${OPTARG};; # dir of pdf files
        o) OUT_DIR=${OPTARG};; # dir of outputs
        esac
done

for i in $PDF_DIR/*; do
#	echo $i > $OUTPUT/temp
	$TOOL $COMMAND $i /dev/null 
#	if grep -q "pdf_field_mark_dirty!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" $OUT_DIR/tmp; then
#  		echo $i >> $OUT_DIR/triggered_files
#	fi;
#	rm -rf $OUT_DIR/tmp
done
 
