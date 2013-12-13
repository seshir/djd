iToggle uses the iToggle jQuery plugin to create engaging and interactive widgets for fields and boolean bundle properties.

CONTENTS OF THIS FILE
---------------------
 * Introduction
 * Installation
 * Authors
 * Sponsors

INTRODUCTION
------------

It does the following:

    * Autodetects entity types and boolean bundle properties
    * Provides an extra Views Field for each Bundle/Property combination
    * Provides a new Field Type for Boolean fields
    * Provides a Field Widget for iToggle Fields
    * Provides a Field Formatter for both iToggle and Boolean Fields
    * Allows iToggle widget to be used for editing node properties: status, promote & sticky

iToggle does not yet do the following

    * Allow generic entity properties to be toggled in entity edit forms
    * Offer any kind of advanced access control for entities outside of core.
        We have a "use itoggle" permission and that's about it for now.

The iToggle widgets can optionally trigger AJAX requests that toggle entity properties or field values.
Permissions are checked and a security token is validated before executing the action to avoid exploits.


INSTALLATION
------------

 * Extract the module in your sites/all/modules or sites/all/modules/contrib directory and enable it
 * Download the iToggle library and put it in your sites/all/libraries directory so that it looks like this:

    sites/all/libraries/engage.itoggle/engage.itoggle.min.js

 * Download the jQuery Easing library and put it in your sites/all/libraries directory so that it looks like this:

    sites/all/libraries/jquery.easing/jquery.easing.1.3.js

 * If you use Drush, you can alternatively download the libraries directly using:

    drush itoggle-download


AUTHORS
-------

* Alex Weber (alexweber) - http://drupal.org/user/850856 & http://www.alexweber.com.br


SPONSORS
--------

This project is made possible by Webdrop (http://webdrop.net.br)