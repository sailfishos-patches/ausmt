find_palm_md5sum() {
  palm_md5sum=""
  pkg_package="test"
  pkg_version="0"

  echo "Finding md5sum for original file" >> ${AUSMT_LOG_FILE}
  echo "  file: ${file}" >> ${AUSMT_LOG_FILE}

  # In the test, we get md5sum from the generated md5sums
  md5sums_line=`grep "^${file} " ${FILES_DIR}/md5sums`
  palm_md5sum=`echo ${md5sums_line} | awk '{print $2}'`

  if [ ! -z "${palm_md5sum}" ]; then
    echo "${file} ${palm_md5sum} ${pkg_package} ${pkg_version}" >>$AUSMT_CACHE_FILE
  fi
}