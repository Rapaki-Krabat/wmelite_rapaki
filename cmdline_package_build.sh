#!/bin/bash

WINEPROJECTPATH="Z:\\var\\lib\\jenkins\\workspace\\wmelite_demo_packaging\\projects\\wme_demo_touch\\wme_demo.wpr"

WINEOUTPUTPATH="Z:\\var\\lib\\jenkins\\workspace\\wmelite_demo_packaging\\projects\\wme_demo_touch\\packages\\"

WINETOOLSPATH="Z:\\var\\lib\\jenkins\\workspace\\wmelite_demo_packaging\\wmetools\\"

# remove old packages
rm -rf /var/lib/jenkins/workspace/wmelite_demo_packaging/projects/wme_demo_touch/packages/

# projectfile outputdir toolsdir addcrashlib enablelogfilewriting
wine ProjectCompiler.exe $WINEPROJECTPATH $WINEOUTPUTPATH $WINETOOLSPATH true true

