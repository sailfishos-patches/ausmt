get_bck_file() {
  bck_file=`dirname ${file}`"/."`basename ${file}`".orig"

  if [ ! -f ${bck_file} ]; then
    bck_file="${file}.webosinternals.orig"
  fi
}