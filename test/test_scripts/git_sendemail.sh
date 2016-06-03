#git_new request-pull 966e0e5cbcc205429e282cd811d07364d14b67d8 .  > commit_log 
git send-email -2				\
		--annotate			\
		--suppress-cc=all		\
		--to=linux-uek_ww@oracle.com	\
		--cc=ryan.ding@oracle.com	

#		--compose			\	# Add introduction email, eg: {PATCH 0/n}
#		--annotate			\	# Edit each patch before send
#		--to=srinivas.eeda@oracle.com	\
#		--to=junxiao.bi@oracle.com	\
#		--cc=joseph.qi@huawei.com	\
#		--cc=akpm@linux-foundation.org	\
#		--cc=mfasheh@suse.de		\
