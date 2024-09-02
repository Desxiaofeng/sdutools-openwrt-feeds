#
# Copyright (C) 2006-2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sdunetd
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/sdunetd
  SECTION:=util
  CATEGORY:=SDUtool
  TITLE:=auto login

endef

define Package/sdunetd/description
 sdunetd. used for sdu.
endef

define Build/Compile
endef

define Package/sdunetd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sdunetd $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/sdutools/sdunetd
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/sdunetd.init $(1)/etc/init.d/sdunetd
endef


$(eval $(call BuildPackage,sdunetd))
