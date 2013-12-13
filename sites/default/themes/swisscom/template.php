<?php

/*
 * Implements hook_preprocess_html().
 */
function swisscom_preprocess_html(&$variables) {
  $header_bg_color         = theme_get_setting('header_bg_color');
  $header_txt_color        = theme_get_setting('header_txt_color');
  $header_hover_bg_color   = theme_get_setting('header_hover_bg_color');
  $header_hover_txt_color  = theme_get_setting('header_hover_txt_color');
  $header_ui_color         = theme_get_setting('header_ui_color');
  $header_login_color      = theme_get_setting('header_login_color');
  $link_color              = theme_get_setting('link_color');
  $link_hover_color        = theme_get_setting('link_hover_color');
  $footer_bg_color         = theme_get_setting('footer_bg_color');
  $footer_link_color       = theme_get_setting('footer_link_color');
  $footer_link_hover_color = theme_get_setting('footer_link_hover_color');
  $button_background_color = theme_get_setting('button_background_color');
  $button_text_color       = theme_get_setting('button_text_color');

  drupal_add_css(".navbar-inner {background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar .nav > li > a {color: $header_txt_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar .nav > li > a:hover, .navbar .nav > li > a.active {background-color: $header_hover_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar .nav .active > a, .navbar .nav .active > a:hover, .navbar.navbar-fixed-top #main-menu li a:hover {background-color: $header_hover_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar .nav > li > a:hover {color: $header_hover_txt_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("#main-menu {background-color: $header_ui_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("#login-buttons {background-color: $header_login_color;width:auto}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("a {color: $link_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("a:hover {color: $link_hover_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".footer .footer-inner {background-color: $footer_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".footer .footer-inner .navbar ul.footer-links > li > a {color: $footer_link_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".footer .footer-inner .navbar ul.footer-links > li > a:hover {color: $footer_link_hover_color}", array('group' => CSS_THEME, 'type' => 'inline'));

  drupal_add_css(".btn {background: $button_background_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".btn {color: $button_text_color}", array('group' => CSS_THEME, 'type' => 'inline'));
}


/**
 * Preprocessor for theme('region').
 */
function swisscom_preprocess_region(&$variables, $hook) {

  if($variables['region'] == "sidebar_first") {
    $variables['classes_array'][] = 'well';
  }

  if ($variables['region'] == "sidebar_second") {
    $parent_id = 0;
    if(isset($variables['elements']['book_navigation'])) {
      foreach ($variables['elements']['book_navigation'] as $element) {
        if (is_array($element) && isset($element['#theme'])) {
            $tmp = explode('_', $element['#theme']);
            $parent_id = $tmp[sizeof($tmp) - 1];
        }
      }

      $parent_info = node_load($parent_id);
      $tmp = $variables['content'];
      $tmp = str_replace('<h2>Topics</h2>', '<h3><a href="/' . $parent_info -> book['link_path'] . '">' . $parent_info -> title . '</a></h3><h2>Topics</h2>', $tmp);
      $variables['content'] = $tmp;
    }
  }
}


/**
 * Overrides theme_menu_link().
 *
 * Prints sub-items with the menu link.
 */
function swisscom_menu_link(array $variables) {
    $element = $variables['element'];
    $sub_menu = '';

    if ($element['#below']) {
        // Add our own wrapper
        unset($element['#below']['#theme_wrappers']);
        $sub_menu = '<ul>' . drupal_render($element['#below']) . '</ul>';

        $element['#localized_options']['html'] = TRUE;

        #$element['#href'] = "";
    }

    $output = l($element['#title'], $element['#href'], $element['#localized_options']);
    return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Preprocessor for theme('page').
 */
function swisscom_preprocess_page(&$variables) {
  // The logo is the spinning icon, so also get "Swisscom" spelled out to add to header.
  global $base_url, $user;
  $variables['logo_name'] = $base_url . '/' . drupal_get_path('theme', 'swisscom') . '/logo-name.png';
  
  // Alter breadcrumb for App Details page
  if(arg(0) == 'user' && arg(2) == 'app-detail' ) {
  	// Build Breadcrumbs
	$owner_name = ($user->uid == $GLOBALS['user']->uid ? 'My' : $user->name . "'s");
	$breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l("$owner_name API keys", 'user/' . $user->uid . '/apps');
	
	drupal_set_breadcrumb($breadcrumb);
  }
}

/**
 * Theme function for unified login page.
 *
 * @ingroup themable
 */
function swisscom_lt_unified_login_page($variables) {

  $login_form = $variables['login_form'];
  $register_form = $variables['register_form'];
  $active_form = $variables['active_form'];
  $output = '';

  $output .= '<div class="toboggan-unified ' . $active_form . '">';

  // Create the initial message and links that people can click on.
  $output .= '<div id="login-links">';
  $output .= l(t('I have an account'), 'user/login', array('attributes' => array('class' => array('login-link'), 'id' => 'login-link')));
  $output .= ' ';
  $output .= l(t('I want to create an account'), 'user/register', array('attributes' => array('class' => array('login-link'), 'id' => 'register-link')));

  $output .= '</div>';

  // Add the login and registration forms in.
  $output .= '<div id="login-form">' . $login_form . '</div>';
  $output .= '<div id="register-form">' . $register_form . '</div>';

  $output .= '</div>';

  return $output;
}

/**
 * Implements template_preprocess_field().
 */
function swisscom_preprocess_field(&$vars) {
  // Add class to API method verb for css processing.
  if ($vars['element']['#field_name'] == 'field_api_method_name') {
    if (isset($vars['element']['#items'][0]['value'])) {
      $api_method_value = $vars['element']['#items'][0]['value'];
      $vars['classes_array'][] = 'api-method-' . $api_method_value;
    }
    else {
      $vars['classes_array'][] = 'api-method-undefined';
    }

  }
}


//function swisscom_breadcrumb($variables) {
//  $links = array();
//  $path = '';
//
//  // Get URL arguments
//  $arguments = explode('/', request_uri());
//
//  // Remove empty values
//  foreach ($arguments as $key => $value) {
//    if (empty($value)) {
//      unset($arguments[$key]);
//    }
//  }
//  $arguments = array_values($arguments);
//
//  // Add 'Home' link
//  $links[] = l(t('home'), '<front>');
//
//  // Add other links
//  if (!empty($arguments)) {
//    foreach ($arguments as $key => $value) {
//      // Don't make last breadcrumb a link
//      if ($key == (count($arguments) - 1)) {
//        $links[] = drupal_strtolower(drupal_get_title());
//      } else {
//        if (!empty($path)) {
//          $path .= '/'. $value;
//        } else {
//          $path .= $value;
//        }
//        $links[] = l(drupal_strtolower($value), $path);
//      }
//    }
//  }
//
//  // Set custom breadcrumbs
//  drupal_set_breadcrumb($links);
//
//  // Get custom breadcrumbs
//  $breadcrumb = drupal_get_breadcrumb();
//
//  // Hide breadcrumbs if only 'Home' exists
//  if (count($breadcrumb) > 1) {
//    return '<div class="breadcrumb">'. implode(' &raquo; ', $breadcrumb) .'</div>';
//  }
//}
