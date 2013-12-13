<?php
/**
 * @file
 * iToggle theme functions.
 */

/**
 * Implements hook_theme().
 */
function itoggle_theme() {
  return array(
    'itoggle' => array(
      'variables' => array(
        'type' => NULL,
        'id' => NULL,
        'property' => NULL,
        'checked' => NULL,
        'scope' => NULL,
        'clickable' => 1,
        'display_type' => 0,
      ),
    ),
  );
}

/**
 * Theme callback.
 *
 * @param array $variables
 *  An array with the folowing keys/values:
 *    - string $type
 *        The entity type
 *    - int $id
 *        The property id
 *    - string $property
 *        The property machine-name
 *    - bool $checked
 *        The property value (true/false)
 *    - string $scope
 *        The widget scope (views/field/inline)
 *    - bool $clickable
 *        Whether the widget is clickable (triggers callback)
 *    - int $display_type
 *        The iToggle display type (0 = on/off, 1 = yes/no, 2 = 1/0)
 *
 * @see itoggle_theme()
 */
function theme_itoggle($variables) {
  static $element_ids;

  extract($variables);
  $checked = ($checked == TRUE) ? 'checked="checked"' : '';

  switch ($display_type) {
    case 0:
      $display_type = 'onoff';
      break;
    case 1:
      $display_type = 'yesno';
      break;
    case 2:
      $display_type = '10';
      break;
  }

  $element_id = "itoggle_{$type}_{$property}_{$id}_";

  if (!isset($element_ids[$element_id])) {
    $element_ids[$element_id] = 1;
  }
  else {
    $element_ids[$element_id]++;
  }

  $element_id .= $element_ids[$element_id];

  return <<<HTML
<div class="itoggle-wrapper itoggle-display-{$display_type} itoggle-{$type} itoggle-{$type}_{$property}" data-id="{$id}" data-type="{$type}" data-scope="{$scope}" data-property="{$property}">
  <input type="checkbox" id="{$element_id}" {$checked} />
</div>
HTML;
}

/**
 * Implements hook_preprocess_hook().
 *
 * Includes javascript files and settings for theme_itoggle().
 *
 * @see itoggle_theme()
 */
function itoggle_preprocess_itoggle(&$variables) {
  extract($variables);

  // Set token key.
  $token_key = "itoggle_{$type}_{$property}_{$id}";

  // Include iToggle javascript & stylesheet.
  itoggle_include_itoggle(TRUE);

  // Must have permission to click iToggle widget.
  if (!user_access('use itoggle')) {
    $clickable = FALSE;
  }

  $settings = array(
    'itoggle' => array(
      'tokens' => array(
        $token_key => drupal_get_token($token_key),
      ),
      'clickable' => array(
        $token_key => (bool) $clickable,
      ),
    )
  );

  // Add an individual token and clickable setting for each input.
  drupal_add_js($settings, 'setting');
}