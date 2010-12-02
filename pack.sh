#!/bin/bash

EXE_NAME=deeptrim
PROJ_NAME=permute
VERSION=`grep "ProgramVersion::VersionNumber" src/version.cpp \
        | awk '{print $3;}' \
        | sed s/[\(\"\;\)]//g`
PACKDIR=${HOME}/packaging/${PROJ_NAME}

makearchive.sh ${EXE_NAME}-${VERSION} master
cp ${EXE_NAME}-${VERSION}.tar.gz ${PACKDIR}
echo ${EXE_NAME} > ${PACKDIR}/pack-name
echo ${VERSION} > ${PACKDIR}/pack-version
ls -l ${PACKDIR}/${EXE_NAME}-${VERSION}.tar.gz
ls -l ${PACKDIR}/pack-*

if [ x$1 == "xmake" ]
then
  cd ${PACKDIR}
  make
fi
