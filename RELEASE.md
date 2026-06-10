# Releasing Firmware

GitHub Actions is set up to automatically build and release firmware.

It will automatically build firmware when one of the following tag formats are pushed.

- `companion-v1.0.0`
- `repeater-v1.0.0`
- `room-server-v1.0.0`

> NOTE: replace `v1.0.0` with the version you want to release as.

- You can push one, or more tags on the same commit, and they will all build separately.
- Once the firmware has been built, a new (draft) GitHub Release will be created.
- You will need to update the release notes, and publish it.

## Commands

Create and push a tag (replace `companion` and version as needed):

```bash
git tag -a "companion-v1.16.0.4" -m "companion-v1.16.0.4"
git push origin "companion-v1.16.0.4"
```

To tag multiple firmware types on the same commit:

```bash
git tag -a "companion-v1.0.0" -m "companion-v1.0.0"
git tag -a "repeater-v1.0.0" -m "repeater-v1.0.0"
git tag -a "room-server-v1.0.0" -m "room-server-v1.0.0"
git push origin --tags
```

To delete a tag locally and remotely (e.g. to re-tag after a fix):

```bash
git tag -d "companion-v1.0.0"
git push origin --delete "companion-v1.0.0"
```
