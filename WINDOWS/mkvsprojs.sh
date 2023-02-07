#!/bin/bash

#IMB-EXT_VS
PROJ=$1
if [ -d ${PROJ}_2017 ]; then rm -rf ${PROJ}_2017; fi

cp -r ${PROJ}_2013 ${PROJ}_2017

VCXPROJ_2017=${PROJ}_2017/*.vcxproj

sed -i 's!ToolsVersion="12.0"!ToolsVersion="15.0"!' $VCXPROJ_2017
sed -i 's!<Keyword>Win32Proj</Keyword>!<Keyword>Win32Proj</Keyword>\n    <WindowsTargetPlatformVersion>10.0.15063.0</WindowsTargetPlatformVersion>!' $VCXPROJ_2017
sed -i 's!Toolset>v120<!Toolset>v141<!' $VCXPROJ_2017
