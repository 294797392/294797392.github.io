#!/bin/bash
# vim: ff=unix :

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

apt-get update
apt-get install -y --force-yes composer				# laravel used
apt-get install -y --force-yes php-mbstring			# php mbstring plugin
apt-get install -y --force-yes php-xml				# php xml plugin
apt-get install -y --force-yes php7.0-curl			# php curl plugin
apt-get install -y --force-yes php7.0-mysql			# mysql driver
apt-get install -y --force-yes unzip
cp .env.example .env
php artisan key:generate
chown www-data:www-data -R .

popd > /dev/null

