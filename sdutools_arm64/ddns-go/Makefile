#
# Copyright (C) 2006-2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ddns-go
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/ddns-go
  SECTION:=util
  CATEGORY:=SDUtool
  TITLE:=ddns

endef

define Package/ddns-go/description
 ddns-go. used for sdu.
endef

define Build/Compile
endef

define Package/ddns-go/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ddns-go $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/sdutools/ddns-go
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ddns-go.init $(1)/etc/init.d/ddns-go
endef


$(eval $(call BuildPackage,ddns-go))
