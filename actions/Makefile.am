DEBUILD_ARGS =
SAVE_CONTAINER = no

clean-local:
	-rm -rf build/

.PHONY: actions_build_deb
actions_build_deb: IMAGE_NAME = debian
actions_build_deb: IMAGE_VERSION = unstable
actions_build_deb: CONTAINER_NAME = $(PACKAGE_NAME)_$@_$(IMAGE_NAME)-$(IMAGE_VERSION)
actions_build_deb: DEPENDENCY_LIST = autoconf-archive debmake devscripts ncurses-dev pkg-config
actions_build_deb: ../$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz
	podman pull docker.io/library/$(IMAGE_NAME):$(IMAGE_VERSION)
	podman container exists "$(CONTAINER_NAME)" || podman run -itd --name "$(CONTAINER_NAME)" "$(IMAGE_NAME):$(IMAGE_VERSION)"
	podman exec "$(CONTAINER_NAME)" mkdir -p /tmp/build
	podman cp "../$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz" "$(CONTAINER_NAME)":/tmp/build
	podman exec -w /tmp/build "$(CONTAINER_NAME)" apt-get -y update
	podman exec -w /tmp/build "$(CONTAINER_NAME)" apt-get -y upgrade
	podman exec -w /tmp/build "$(CONTAINER_NAME)" apt install -y $(DEPENDENCY_LIST)
	podman exec -w /tmp/build "$(CONTAINER_NAME)" debmake -y -a $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz -i debuild $(DEBUILD_ARGS)
	mkdir -p "build/$@/$(IMAGE_NAME)-$(IMAGE_VERSION)"
	podman cp "$(CONTAINER_NAME)":/tmp/build/. "build/$@/$(IMAGE_NAME)-$(IMAGE_VERSION)/"
	if [ "$(SAVE_CONTAINER)" = "no" ]; then podman kill "$(CONTAINER_NAME)" && podman rm "$(CONTAINER_NAME)"; fi
