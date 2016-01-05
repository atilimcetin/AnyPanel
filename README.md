This project is **OBSOLETE**

# AnyPanel

AnyPanel is an easy to use heads up display for Mac OS X to provide at-a-glance information. 

<img src="screenshot.png?raw=true" />

This project is mostly inspired by [AnyBar](https://github.com/tonsky/AnyBar).

## Download

Version 0.1.0:

<a href="https://github.com/atilimcetin/AnyPanel/releases/download/0.1.0/AnyPanel-0.1.0.zip">AnyPanel-0.1.0.zip</a>


## Usage

After launching AnyPanel.app, it starts displaying a grid and you can change the value of a cell by sending a text message through UDP (port is 6128 by default).

```sh
echo -n "1,1 hello world" | nc -4u -w0 localhost 6128   # display "hello world" at cell (1, 1)
echo -n "2,3 FizzBuzz"    | nc -4u -w0 localhost 6128   # display "FizzBuzz" at cell (2, 3)
```

It's also possible to use ANSI terminal codes to set text and background color:

```sh
echo -n -e "2,2 \033[31mFizz\033[32mBuzz" | nc -4u -w0 localhost 6128   # display red "Fizz" and green "Buzz" at cell (2, 2)
```

The application window is semi-transparent, always on top and does not intercept mouse clicks.

## Customization

Once AnyPanel.app is launched, it creates a JSON configuration file at `~/Library/Application Support/AnyPanel/preferences.json`. The configuration file stores the UDP port, window opacity, coordinates of the application, grid size, grid color, border style, etc. If the contents of the configuration file is changed while AnyPanel.app is running, the app automatically reads the configuration file and applies the changes.

The grid displayed is basically an HTML file. While customizing the app, you can use HTML color codes and CSS styles.


## License

Copyright © 2015 Atilim Cetin

Licensed under MIT License (see [LICENSE](LICENSE)).
