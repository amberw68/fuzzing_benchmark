while getopts t:c:p:o:s: option
do
        case "${option}"
        in
        t) TOOL=${OPTARG};; # Tool binary you want to test
        c) COMMAND=${OPTARG};; # Tool needed commands or options
        p) PDF_DIR=${OPTARG};; # dir of pdf files
        o) OUT_DIR=${OPTARG};; # dir of outputs
	s) TAR_STR=${OPTARG};; # the target string you want to monitor
        esac
done

cnt=0

for i in $PDF_DIR/*; do

	echo $cnt

	let "cnt = cnt + 1"

	$TOOL $COMMAND $i > $OUT_DIR/$cnt;

        if grep -q "$TAR_STR" $OUT_DIR/$cnt; then
               echo $i >> $OUT_DIR/triggered_files;
        fi;
        #rm -rf $OUT_DIR/see_temp;
done
