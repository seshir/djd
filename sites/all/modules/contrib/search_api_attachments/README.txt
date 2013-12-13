Search API Attachments

This module will extract the content out of attached files using the Tika library and index it.
Search API attachments will index many file formats, including PDF's, MS Office, MP3 (ID3 tags), JPEG Metadata, ...

The module was tested with Apache Solr, however it should work on all Search API search servers.
Database Search gives errors on saving but should work (Core Issue: http://drupal.org/node/1007830)

More information:
http://tika.apache.org/download.html

REQUIREMENTS
------------

Requires the ability to run java on your server and an installation of the Apache Tika library


INSTALLATION
------------

On Ubuntu 10.10

Install java
> sudo apt-get install openjdk-6-jdk

Download Apache Tika library: http://tika.apache.org/download.html
> wget http://apache.megamobile.be//tika/tika-app-1.0.jar

Copy search_api_attachments into your modules folder

Install the search_api_attachments module in your Drupal site

Go to the configuration: admin/config/search/search_api/attachments

Enter the full path on your server where you downloaded the jar e.g. /var/apache-tika/ 
and the name of the jar file e.g. tika-app-1.0.jar