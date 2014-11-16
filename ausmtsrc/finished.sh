failure() {
  failure_unpatch

  echo | tee -a ${AUSMT_LOG_FILE}
  echo "*** FAILED ***" | tee -a ${AUSMT_LOG_FILE}
  echo | tee -a ${AUSMT_LOG_FILE}

  failure_notify

  rm -f $AUSMT_CACHE_FILE >/dev/null 2>&1

#   mount -o ro,remount / | tee -a ${AUSMT_LOG_FILE} 1>&2

  exit 1
}

success() {
  echo | tee -a ${AUSMT_LOG_FILE}
  echo "*** SUCCESS ***" | tee -a ${AUSMT_LOG_FILE}
  echo | tee -a ${AUSMT_LOG_FILE}

  success_notify

  rm -f $AUSMT_CACHE_FILE >/dev/null 2>&1

#   mount -o ro,remount / | tee -a ${AUSMT_LOG_FILE} 1>&2

  exit 0
}