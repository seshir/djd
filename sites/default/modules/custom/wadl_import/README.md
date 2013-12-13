cs_wadl_import
==============

A custom CS module that allows the import of WADL for API documentation. 

## Installation

Follow the following steps:

 1. Install the module in the `/sites/all/modules/custom` directory.  If the
    custom directory does not exist, you mya need to create it.
 2. Log in as an administrator and enable the "WADL Import" module.
 3. Flush all caches by logging in as an admin and clicking
    on the icon in the far left corner, then clicking on "Flush all caches". 

## Overview

__NOTE__: This module takes some setup by someone familiar with common Drupal concepts such
as Taxonomy and Views. 

The module creates the following:

* An "API Name" and "API Tag" taxonomy under "Structure -> Taxonomy"
* An "API Attributes" block view, which can be seen under "Structure -> Blocks" and
  can be configured from the view at "Structure -> Views".
* An "API Documentation Overview" view page that can configured at "Structure -> Views".
* A new content type named "API Resource Doc".

## First time setup

This module takes advantage of Drupal taxonomy, views, content types, and blocks to be
very configurable.  This section describes the first time setup, but is very dependent
on how you want your documentation set up.

1. Create new API names under the taxonomy vocabulary "API Name".  You need at least
one API to start.
2. Import a WADL or manually create a few test methods to see how the system works.
See the sections below to learn how to create a method by hand or import a WADL.
3. Once you have created one or more "API Resource Doc" nodes, you can see the a list
of APIs by going to <hostname>/api, which is the "API Documentation Overview". You
will want to make links to this page so that people can see your API overview.
4. If you want to put some attributes in a sidebar, you will need to configure the
"API Attributes" block view to display those attributes and only display that block
on api/* pages. You will see that all "API Resource Doc" nodes are located under
/api.  This can be configured in the URL aliases module located at "Configuration ->
Search and metadata ->  URL Aliases, then click on the "Patterns" tab.

## Importing a WADL

You can import a WADL by logging in as an administrator and clicking on
"Content -> WADL API Documentation Import". 

## Changes

_1.1:_

* Added API Name - each WADL you import can be tied to the "API Name"
     taxonomy.
* Added new method attributes "type" and "style"
