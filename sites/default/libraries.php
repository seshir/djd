<?php
/**
 * @file
 *   Describes libraries' download URLs so that drush apigee-release can put
 *   the correct info in the makefile.
 *
 * Each entry in $libraries_info should be either a URL string or an
 * associative array with 'url' and 'tag' elements. drush apigee-release is
 * smart enough to figure out the download type (git, get, etc.) based on the
 * URL.
 */

$libraries_info = array(
  'awssdk' => 'http://pear.amazonwebservices.com/get/sdk-latest.zip',
  'backbone' => array( 'url' => 'git://github.com/documentcloud/backbone.git', 'tag' => '0.9.2' ),
  'ckeditor' => 'http://download.cksource.com/CKEditor/CKEditor/CKEditor%203.6.4/ckeditor_3.6.4.zip',
  'codemirror' => 'http://codemirror.net/codemirror.zip',
  'colorpicker' => 'http://www.eyecon.ro/colorpicker/colorpicker.zip',
  'glip' => 'git://github.com/halstead/glip.git',
  'highcharts' => 'http://www.highcharts.com/downloads/zips/Highcharts-2.2.5.zip',
  'jquery.cycle' => 'http://www.malsup.com/jquery/cycle/release/jquery.cycle.zip?v2.86',
  'jquery.selectlist' => 'http://odyniec.net/projects/selectlist/jquery.selectlist-0.6.1.zip',
  'json2' => 'git://github.com/douglascrockford/JSON-js.git',
  'mediaelement' => 'https://nodeload.github.com/johndyer/mediaelement/zipball/master',
  'oauth-php' => 'http://oauth-php.googlecode.com/files/oauth-php-175.tar.gz',
  'prettify' => 'http://google-code-prettify.googlecode.com/files/prettify-small-4-Mar-2013.tar.bz2',
  's3-php5-curl' => 'http://amazon-s3-php-class.googlecode.com/files/s3-php5-curl_0.4.0.tar.gz',
  'SolrPhpClient' => 'http://solr-php-client.googlecode.com/files/SolrPhpClient.r60.2011-05-04.zip',
  'spyc' => 'http://spyc.googlecode.com/files/spyc-0.5.zip',
  'tinymce' => 'http://cloud.github.com/downloads/tinymce/tinymce/tinymce_3.4.7.zip',
);
