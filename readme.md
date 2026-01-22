**Due to the discontinuation of support for the `discord-rpc` repository, I will have to write my own Discord RPC library. This will take some time, but it will have proper support and regular updates.**

# Terminal-Based DiscordRPC Program
This program is designed to let you create your own Discord RPC easily and quickly прямо from the Linux terminal in an optimized way.

## Installation
To install this program, you will need the `discord-rpc` library, which is also included in this repository. You may need to spend some time compiling it.

> You can easily install this program using `xbps-src` and my template, which is available in the repository under the same name. The link will appear later after the project is fully completed.

For installation, a `Makefile` (`make`) is used. The instructions are provided below:
```bash
make                            # default build mode
make build                      # build the project
sudo make install               # install to `/usr/local` by default
make install PREFIX=/usr        # install to a custom path instead of the default
sudo make uninstall             # uninstall from `/usr/local` by default
sudo make uninstall PREFIX=/usr # uninstall from a custom path
```

## Found a Bug?
Report any errors or issues in the **Issues** section and help us improve the program. Let’s make it better together!

## License
This project is distributed under the MIT License.
