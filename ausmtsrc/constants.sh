# Root
#include constants_root.sh

# Paths / Files
AUSMT_CONTROL_DIR=${AUSMT_VAR_DIR}/control
PATCH_DIR=${PATCH_ROOT_DIR}/${1}

AUSMT_LOG_FILE=${AUSMT_VAR_DIR}/ausmt.log
AUSMT_PACKAGES_LIST_FILE=${AUSMT_VAR_DIR}/packages
AUSMT_CACHE_FILE=${AUSMT_VAR_DIR}/packages.cache
AUSMT_PATCH_BACKUP_ROOT_DIR=${AUSMT_VAR_DIR}/patches
AUSMT_PATCH_BACKUP_DIR=${AUSMT_PATCH_BACKUP_ROOT_DIR}/${1}

LIPSTICK_FILE=/usr/bin/lipstick
PANDORA_MD5SUMS_FILE=/opt/lipstick-pandora/md5sums

# Constants
PATCH_NAME=unified_diff.patch
VERSION=ausmt-sailfishos-1

#include constants_exec.sh
