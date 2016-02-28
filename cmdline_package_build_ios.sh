#!/bin/bash

# WINEPROJECTPATH="Z:\\var\\lib\\jenkins\\workspace\\Krabat2\\deploy\\K2Deploy\\game\\K2.wpr"

# build the touch project, this includes fixes and thus will build all packages
WINEPROJECTPATH="Z:\\Users\\Shared\\Jenkins\\Home\\jobs\\wmelite_demo_packaging\\workspace\\projects\\wme_demo_touch\\wme_demo.wpr"

WINEOUTPUTPATH="Z:\\Users\\Shared\\Jenkins\\Home\\jobs\\wmelite_demo_packaging\\workspace\\projects\\wme_demo_touch\\packages\\"

WINETOOLSPATH="Z:\\Users\\Shared\\Jenkins\\Home\\jobs\\wmelite_demo_packaging\\workspace\\wmetools\\"

# already done

#UNIXOUTPUTPATH=/var/lib/jenkins/workspace/Krabat2/deploy/

#rm -rf $UNIXOUTPUTPATH

# projectfile outputdir toolsdir addcrashlib enablelogfilewriting
/Applications/Wine\ Staging.app/Contents/Resources/wine/bin/wine ProjectCompiler.exe $WINEPROJECTPATH $WINEOUTPUTPATH $WINETOOLSPATH true true

