get_palm_md5sum() {
  palm_md5sum=""
  pkg_package=""
  pkg_version=""

  appid=`basename ${PATCH_DIR}`

  if [ -f $AUSMT_CACHE_FILE ]; then
    md5sums_line=`grep "^${file} " $AUSMT_CACHE_FILE`

    palm_md5sum=`echo ${md5sums_line} | awk '{print $2}'`
    pkg_package=`echo ${md5sums_line} | awk '{print $3}'`
    pkg_version=`echo ${md5sums_line} | awk '{print $4}'`
  fi
} 
