<?php

/**
 * @file
 * Hansel breadcrumb actions
 *
 * Breadcrumb actions from Hansel core are located in this file.
 * The definitions resides in hansel.module
 *
 * @see hansel_hansel_action_types()
 */

/**
 * Callback for the "add single link" breadcrumb action to generate the crumbs.
 *
 * @param array $arguments
 * @return array
 */
function hansel_action_add_single_link_get_crumbs($arguments) {
  if (isset($arguments['translate']) && $arguments['translate']) {
    $title = t($arguments['title']);
  }
  else {
    $title = $arguments['title'];
  }
  if (!empty($arguments['remove_tokens'])) {
    $title = preg_replace('/\\[[a-z0-9_\\-]+\\]/si', '', $title);
  }
  return array(
    array(
      'title' => $title,
      'href' => $arguments['path'],
    ),
  );
}

/**
 * Callback for the "add single link" breadcrumb action to generate the info line.
 *
 * @param array $arguments
 * @return string
 */
function hansel_action_add_single_link_info($arguments) {
  return t('add link %title', array('%title' => $arguments['title']));
}

/**
 * Callback for the "add link to node" breadcrumb action to generate the crumbs.
 *
 * @param array $arguments
 * @return array
 */
function hansel_action_add_link_to_node_get_crumbs($arguments) {
  if (drupal_strtolower(hansel_arg(0)) == 'node' && is_numeric(hansel_arg(1))) {
    if ($node = node_load(hansel_arg(1))) {
      return array(
        array(
          'title' => $node->title,
          'href' => 'node/' . $node->nid,
        ),
      );
    }
  }
  return array();
}

/**
 * Callback for the "add link to nodetype" breadcrumb action to generate the crumbs.
 *
 * @param array $arguments
 * @return array
 */
function hansel_action_add_link_to_nodetype_get_crumbs($arguments) {
  if (drupal_strtolower(hansel_arg(0)) == 'node' && is_numeric(hansel_arg(1))) {
    if ($node = node_load(hansel_arg(1))) {
      $type = $node->type;
    }
  }
  elseif (drupal_strtolower(hansel_arg(0)) == 'node' && drupal_strtolower(hansel_arg(1)) == 'add') {
    // Nodetypes may not contain hyphens. So it's same to assume that all hyphens are underscores.
    $type = str_replace('-', '_', hansel_arg(2));
  }
  if (!empty($type)) {
    $path = isset($arguments['path']) ? $arguments['path'] : 'node/add/[type]';
    $path = str_replace('[type]', str_replace('_', '-', $type), $path);
    $path = str_replace('[type-raw]', $type, $path);
    $node = new stdClass();
    $node->type = $type;
    if ($title = node_type_get_name($node)) {
      return array(
        array(
          'title' => t($title),
          'href' => $path,
        ),
      );
    }
  }
  return array();
}

/**
 * Callback for the "add link to user" breadcrumb action to generate the crumbs.
 *
 * @param array $arguments
 * @return array
 */
function hansel_action_add_link_to_user_get_crumbs($arguments) {
  global $user;
  $links = array();

  // Check if we have to use the realname integration.
  if (module_exists('realname')) {
    $use_realname = (isset($arguments['realname']) && $arguments['realname']);
  }
  else {
    $use_realname = FALSE;
  }

  if (drupal_strtolower(hansel_arg(0)) == 'user' && is_numeric(hansel_arg(1))) {
    if (!empty($arguments['you']) && $user->uid == hansel_arg(1) && $user->uid) {
      $links[] = array(
        'title' => (isset($arguments['translate']) && $arguments['translate']) ? t($arguments['you']) : $arguments['you'],
        'href' => 'user/' . $user->uid,
      );
    }
    else {
      $cid = 'user:' . hansel_arg(1) . ':' . ($use_realname ? 'real' : 'std');
      if ($data = hansel_cache_get($cid)) {
        return $data;
      }
      if ($account = user_load(hansel_arg(1))) {
        $links[] = array(
          'title' => $use_realname ? $account->realname : $account->name,
          'href' => 'user/' . $account->uid,
        );
      }
      hansel_cache_set($cid, $links);
    }
  }
  elseif (drupal_strtolower(hansel_arg(0)) == 'node' && is_numeric(hansel_arg(1))) {
    if ($node = node_load(hansel_arg(1))) {
      if (!empty($arguments['you']) && $node->uid == $user->uid && $node->uid) {
        $links[] = array(
          'title' => (isset($arguments['translate']) && $arguments['translate']) ? t($arguments['you']) : $arguments['you'],
          'href' => 'user/' . $user->uid
        );
      }
      else {
        $cid = 'user:' . hansel_arg(1) . ':' . ($use_realname ? 'real' : 'std');
        if ($data = hansel_cache_get($cid)) {
          return $data;
        }
        if ($account = user_load($node->uid)) {
          $links[] = array(
            'title' => $use_realname ? $account->realname : $account->name,
            'href' => 'user/' . $account->uid,
          );
        }
        hansel_cache_set($cid, $links);
      }
    }
  }
  return $links;
}

/**
 * Callback for the "add link to current page" breadcrumb action to generate the crumbs.
 *
 * @param array $arguments
 * @return array
 */
function hansel_action_add_link_to_current_page_get_crumbs($arguments) {
  $args = array();
  $i = 0;
  while ($arg = hansel_arg($i++)) {
    $args[] = $arg;
  }
  $title = drupal_get_title();

  // Prevent title from double encoding
  $title = decode_entities($title);
  if (!empty($title)) {
    return array(
      array(
        'title' => $title,
        'href' => implode('/', $args),
      )
    );
  }
}
