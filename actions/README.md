### Actions

The action targets are designed to automate testing and package building using containerization (via podman). Before running any actions, ensure that `make dist` has been run in the project root.

```
Usage: make TARGET [OPTIONS]

targets:
  actions_build_deb    build a deb package
  actions_build_rpm    build a rpm package

options:
  CONTAINER_NAME=...         specify a container name
  DEBUILD_ARGS  =...         call debuild with additional arguments
  IMAGE_NAME    =...         build container from a specific image (must also set IMAGE_VERSION)
  IMAGE_VERSION =...         build container from a specific image version (must also set IMAGE_NAME)
  SAVE_CONTAINER=<yes/no>    if set to yes, do not remove the podman container after finishing (default is no)
```
