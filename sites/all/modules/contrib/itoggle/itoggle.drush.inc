<?php
/**
 * @file
 * iToggle Drush commands.
 */

/**
 * Implements hook_drush_command().
 */
function itoggle_drush_command() {
  return array(
    'itoggle-download' => array(
      'description' => 'Download the iToggle and jQuery Easing JavaScript libraries to your sites/all/libraries directory.',
      'aliases' => array('itoggle-dl'),
    ),
  );
}

/**
 * Implements hook_drush_help().
 */
function itoggle_drush_help($section) {
  switch ($section) {
    case 'drush:itoggle-download':
      return dt('Download the iToggle and jQuery Easing JavaScript libraries to your sites/all/libraries directory.');
  }
}

/**
 * Drush command.
 * Downloads the iToggle javascript library to the sites/all/libraries directory.
 *
 * @see itoggle_drush_command()
 */
function drush_itoggle_download() {
  // Check to see if Drush Make exists. It is part of Drush core in 5.0-rc2 and up.
  // We use Drush Make to avoid having to re-implement the download logic ourselves.
  // This will only work with Drush 5.0-rc2 or newer.
  // @link http://drupal.org/project/drush
  if (!function_exists('make_download_factory')) {
    drush_die(dt('iToggle requires a newer version of Drush. Please update Drush to 5.0-rc2 or newer: http://drupal.org/project/drush'));
  }

  // First download iToggle library.
  $path = 'sites/all/libraries/engage.itoggle';

  if (!file_exists($path)) {
    // Try to create directory.
    $return = drush_op_system("mkdir -p {$path}");

    // No go, ask user to create directory manually.
    if ($return !== 0) {
      drush_die(dt('The directory "sites/all/libraries/engage.itoggle" does not exist and could not be created due to permissions. Please create it manually and try again.'));
    }
  }

  // Notify user we are starting to download the component.
  drush_print(dt("\nBeginning download of iToggle library.\n"));

  // Create download array for Drush Make.
  $download = array(
    'type' => 'get',
    'url' => 'http://labs.engageinteractive.co.uk/itoggle/engage.itoggle.zip',
  );

  $dir = make_download_file('iToggle', $download, $path);

  // Display success message.
  drush_print(dt("\niToggle library successfully downloaded and installed to !dir.", array('!dir' => $dir)));

  // Then download Easing library.
  $path = 'sites/all/libraries/jquery.easing';

  if (!file_exists($path)) {
    // Try to create directory.
    $return = drush_op_system("mkdir -p {$path}");

    // No go, ask user to create directory manually.
    if ($return !== 0) {
      drush_die(dt('The directory "sites/all/libraries/jquery.easing" does not exist and could not be created due to permissions. Please create it manually and try again.'));
    }
  }

  // Notify user we are starting to download the component.
  drush_print(dt("\nBeginning download of jQuery Easing library.\n"));

  // Create download array for Drush Make.
  $download = array(
    'type' => 'file',
    'url' => 'http://gsgd.co.uk/sandbox/jquery/easing/jquery.easing.1.3.js',
  );

  $dir = make_download_file('jQuery Easing', $download, $path);

  // Make sure its readable.
  drush_op_system("chmod 644 {$path}/jquery.easing.1.3.js");

  // Display success message.
  drush_print(dt("\njQuery Easing library successfully downloaded and installed to !dir.\n", array('!dir' => $dir)));
}