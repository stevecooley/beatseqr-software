# Claude Code Permissions

## Auto-Allowed (no prompt required)

These are configured in `.claude/settings.local.json` for this project:

| Permission | Scope |
|---|---|
| `arduino-cli board` commands | List/detect connected boards |
| `arduino-cli compile` commands | Compile the firmware |
| `arduino-cli upload` commands | Flash firmware to the device |
| `arduino-cli lib` commands | Manage Arduino libraries |
| `Bash(echo "EXIT:$?")` | Read exit codes from shell commands |
| Read Adafruit SAMD library files | `~/Library/Arduino15/packages/adafruit/hardware/samd/1.7.17/libraries/**` |
| Read Arduino library files | `~/Documents/Arduino/libraries/**` |

## Require User Approval (prompt shown)

Everything not listed above will prompt the user before executing:

- All other `Bash` commands (git, find, system commands, etc.)
- `Write` — creating new files
- `Edit` — modifying existing files
- `Read` — reading files outside the explicitly allowed library paths (though in practice project files are routinely approved)
- `WebFetch` / `WebSearch` — network requests
- Spawning background agents

## Never Allowed (hardcoded restrictions)

Regardless of settings, Claude Code will not:

- Generate destructive techniques, DoS attacks, or mass-targeting tools
- Help evade detection for malicious purposes
- Assist with supply chain compromise
- Push to remote git repositories without explicit confirmation
- Run `git push --force` to main/master
- Skip commit hooks (`--no-verify`) without explicit request

## Notes

- The `/flash` skill (arduino-cli compile + upload) runs automatically because both `compile` and `upload` are pre-approved.
- File reads/writes within the project directory will prompt if not pre-approved, but the user can approve them per-session or add them to `settings.local.json`.
- To pre-approve additional commands, add them to `.claude/settings.local.json` in the `permissions.allow` array.
