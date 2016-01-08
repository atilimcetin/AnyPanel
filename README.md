# AnyPanel

AnyPanel is an easy to use heads up display for Mac OS X to provide at-a-glance information.

<img src="AnyPanel/resources/screenshot.png?raw=true" />

This project is mostly inspired by [AnyBar](https://github.com/tonsky/AnyBar) and [BitBar](https://github.com/matryer/bitbar).

## Download

Version 0.1.5:

<a href="https://github.com/atilimcetin/AnyPanel/releases/download/0.1.5/AnyPanel-0.1.5.zip">AnyPanel-0.1.5.zip</a>

## Usage

After launching AnyPanel.app, it starts running a bunch of scripts periodically and displays the output of these scripts on a grid. You can configure the scripts by modifiying the JSON configuration file at `~/Library/Application Support/AnyPanel/preferences.json`.

You can use ANSI terminal codes to set text and background color:

```sh
echo -n -e "\033[31mFizz\033[32mBuzz" # display red "Fizz" and green "Buzz"
```

The application window is semi-transparent, always on top and does not intercept mouse clicks.

## Example scripts

You can find some example scripts at [Scripts](Scripts) directory.

## Customization

The configuration file at `~/Library/Application Support/AnyPanel/preferences.json` stores the window opacity, coordinates of the application, grid size, grid color, border style and scripts. If the contents of the configuration file is changed while AnyPanel.app is running, the app automatically reads the configuration file and applies the changes.

The grid displayed is basically an HTML file. While customizing the app, you can use HTML color codes and CSS styles, etc.


## License

Copyright © 2015 Atilim Cetin

Licensed under MIT License (see [LICENSE](LICENSE)).
