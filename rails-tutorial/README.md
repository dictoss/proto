# try rails tutorital

## document

- https://railstutorial.jp/


## OS environment

- Debian GNU/Linux 9 amd64
- mariadb-server-10.1
- ruby-2.3 (offical debian package)
- rails-5.1.2
- sqlite3 (offical debian package)


## make environment

- `$ sudo apt-get install git`

- `$ sudo apt-get install ruby rubygems ruby-dev gcc make`

- `$ sudo apt-get install zlib1g-dev sqlite3 libsqlite3-dev`

- `$ sudo gem install rails -v 5.1.2`


## generate rails application template

- `$ mkdir path/to/dir`

- `$ cd path/to/dir`

- `$ rails _5.1.2_ new hello_app`

  - many install gems.

- `$ ls hello_app`


## exec develop httpd server

- `$ cd hello_app`

- `$ rails s`

  - error occurred.

    - /var/lib/gems/2.3.0/gems/bundler-1.15.3/lib/bundler/runtime.rb:85:in `rescue in block (2 levels) in require': There was an error while trying to load the gem 'uglifier'. (Bundler::GemRequireError)
    - Gem Load Error is: Could not find a JavaScript runtime. See https://github.com/rails/execjs for a list of available runtimes.

- `$ sudo gem install therubyracer`

  - require g++, exec `$ sudo apt-get install g++`

- add Gemfile at last.

  - gem 'therubyracer'

- `$ sudo bundle install`

- `$ rails s -b 0.0.0.0`

  - => Booting Puma
  - => Rails 5.1.2 application starting in development on http://0.0.0.0:3000
  - => Run `rails server -h` for more startup options

- access web browser, 'http://youripaddress:3000'

  - Yay! You’re on Rails!

