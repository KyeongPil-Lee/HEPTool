#!/bin/bash

python create_fileList.py \
-d /dcache_mnt/dcache/pnfs/iihe/cms/store/user/kplee/MuonHLTTree_phase2MTD_131X_v2/RelValZMM_14 \
-o ../Catalog/RelValZMM.txt

python create_fileList.py \
-d /dcache_mnt/dcache/pnfs/iihe/cms/store/user/kplee/MuonHLTTree_phase2MTD_131X_v2/RelValTTbar_14TeV \
-o ../Catalog/RelValTTbar.txt

python create_fileList.py \
-d /dcache_mnt/dcache/pnfs/iihe/cms/store/user/kplee/MuonHLTTree_phase2MTD_131X_v2/RelValQCD_Pt20toInfMuEnrichPt15_14 \
-o ../Catalog/RelValQCDMuEnriched.txt

echo "done"