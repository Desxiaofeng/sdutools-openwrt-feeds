#
# Copyright (C) 2006-2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=electricity-quiry
PKG_RELEASE:=1
PKG_BUILD_DEPENDS:=curl

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=util
  CATEGORY:=SDUtool
  TITLE:=auto quiry electricity
  DEPENDS:=+libcurl +libstdcpp
endef

define Package/$(PKG_NAME)/description
 $(PKG_NAME). used for sdu.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CXX="$(TARGET_CXX)" \
		CXXFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(TARGET_CONFIGURE_OPTS)
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/sdutools/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
endef


$(eval $(call BuildPackage,$(PKG_NAME)))
