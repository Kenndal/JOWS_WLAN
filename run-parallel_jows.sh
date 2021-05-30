#!/usr/bin/scl enable devtoolset-8 rh-python36 -- /bin/bash -l
N3PATH=$HOME/ns-3-dev
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/students/jprzybyl/ns-3-dev/build/lib
cd $HOME/ns-3-dev
cd build/scratch

# n
# 802.11n, HD, IPTV: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=false --vi_resolution=HD --vi_as_BE=true --standard=n

# 802.11n, HD, IPTV: VI 
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=false --vi_resolution=HD --vi_as_BE=false --standard=n

# 802.11n, HD, IPTV: BE, BK: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=true --vi_resolution=HD --vi_as_BE=true --standard=n

# 802.11n, HD, IPTV: VI, BK: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=true --vi_resolution=HD --vi_as_BE=false --standard=n

# 802.11n, UHD, IPTV: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=false --vi_resolution=UHD --vi_as_BE=true --standard=n

# 802.11n, UHD, IPTV: VI
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=false --vi_resolution=UHD --vi_as_BE=false --standard=n

# 802.11n, UHD, IPTV: BE, BK: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=true  --vi_resolution=UHD --vi_as_BE=true --standard=n

# 802.11n, UHD, IPTV: VI, BK: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=true  --vi_resolution=UHD --vi_as_BE=false --standard=n

# ac
# 802.11ac, HD, IPTV: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=6 --VI=true --BE=false --vi_resolution=HD --vi_as_BE=true --standard=ac

# 802.11ac, HD, IPTV: VI
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=6 --VI=true --BE=false --vi_resolution=HD --vi_as_BE=false --standard=ac

# 802.11ac, HD, IPTV: BE, BK: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=6 --VI=true --BE=true --vi_resolution=HD --vi_as_BE=true --standard=ac

# 802.11ac, HD, IPTV: VI, BK: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=6 --VI=true --BE=true --vi_resolution=HD --vi_as_BE=false --standard=ac

# 802.11ac, UHD, IPTV: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=false --vi_resolution=UHD --vi_as_BE=true --standard=ac

# 802.11ac, UHD, IPTV: VI
#./wlan_jows --RngRun=94 --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=false --vi_resolution=UHD --vi_as_BE=false --standard=ac

# 802.11ac, UHD, IPTV: BE, BK: BE
#./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=true --vi_resolution=UHD --vi_as_BE=true --standard=ac

# 802.11ac, UHD, IPTV: VI, BK: BE
./wlan_jows --calcStart=5 --simTime=30 --nSTA=$SLURM_ARRAY_TASK_ID --nSTA_background=4 --VI=true --BE=true --vi_resolution=UHD --vi_as_BE=false --standard=ac


