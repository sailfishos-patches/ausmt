find_palm_md5sum() {
  palm_md5sum=""
  pkg_package=""
  pkg_version=""

  echo "Finding md5sum for original file" >> ${AUSMT_LOG_FILE}
  echo "  file: ${file}" >> ${AUSMT_LOG_FILE}

  # We change from Palm, and use rpm to query the MD5 (or SHA256) sum
  # Check if the file is in an RPM
  if [ ! -z "$(${RPM_EXEC} -q --queryformat '[%{FILENAMES}\n]' -f ${file} | grep ^${file})" ]; then
    pkg_package=$(${RPM_EXEC} -q --queryformat '%{NAME}' -f ${file} 2> /dev/null)
    pkg_version=$(${RPM_EXEC} -q --queryformat '%{VERSION}-%{RELEASE}' -f ${file} 2> /dev/null)
    palm_md5sum=$(${RPM_EXEC} -q --queryformat '[%{FILENAMES} %{FILEMD5S}\n]' -f ${file} | grep ${file} | awk '{print $2}' 2> /dev/null)
  fi

  if [ ! -z "${palm_md5sum}" ]; then
    echo "${file} ${palm_md5sum} ${pkg_package} ${pkg_version}" >>$AUSMT_CACHE_FILE
  fi
}