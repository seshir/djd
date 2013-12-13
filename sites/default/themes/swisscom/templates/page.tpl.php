<?php

/**
 * @file
 * Default theme implementation to display a single Drupal page.
 *
 * The doctype, html, head and body tags are not in this template. Instead they
 * can be found in the html.tpl.php template in this directory.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['help']: Dynamic help text, mostly for admin pages.
 * - $page['highlighted']: Items for the highlighted content region.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the first sidebar.
 * - $page['sidebar_second']: Items for the second sidebar.
 * - $page['header']: Items for the header region.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see html.tpl.php
 */
?>
<?php global $user; $current_path = implode("/", arg()); ?>
<header id="navbar" role="banner" class="navbar navbar-fixed-top">
  <div class="navbar-inner">
    <div class="container">
      <?php if ($logo): ?>
        <a class="brand" href="<?php print $front_page; ?>" title="<?php print t('Home');  ?>"  tabindex="-1">
          <div class="logo">
            <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" id="logo-swisscom-image"  />

          </div>
          <img src="<?php print $logo_name; ?>" alt="<?php print t('Home'); ?>" id="logo-swisscom-name"  />
        </a>
      <?php endif; ?>
      <div class="nav-collapse">
        <nav role="navigation">
          <?php if ($main_menu): ?>
            <?php print theme('links__system_main_menu',
              array(
                'links' => $main_menu,
                'attributes' => array('id' => 'main-menu','class' => array('nav')),
              )); ?>
          <?php endif; ?>
          <div id='login-buttons' class="span7 pull-right">
            <ul class="nav pull-right">
            <?php if ($user->uid == 0):  ?>
            <!-- show/hide login and register links depending on site registration settings -->
            <?php if (($user_reg_setting != 0) || ($user->uid == 1)): ?>
              <li class="<?php echo (($current_path == "user/register")?"active":""); ?>"><?php echo l(t("register"), "user/register"); ?></li>
              <li class="<?php echo (($current_path == "user/login")?"active":""); ?>"><?php echo l(t("login"), "user/login"); ?></li>
            <?php endif; ?>
            <?php else: ?>
              <?php $user_url = "user/" . $user->uid; ?>
              <li class="dropdown">
                <a href="#" class="dropdown-toggle" data-toggle="dropdown" title="<?php print $user->mail; ?>">
                  <?php print $truncated_user_email; ?><b class="caret"></b>
                </a>
                <ul class="dropdown-menu">
                  <?php if (module_exists('devconnect_developer_apps')): ?>
                  <li><i class="icon-pencil"></i><?php echo l(t('My API Keys'), $user_url . '/apps'); ?></li>
                  <?php endif; ?>
                  <li><i class="icon-user"></i><?php echo l(t('Edit Profile'), $user_url . '/edit'); ?></li>
                  <li><i class="icon-off"></i><?php echo l(t("Logout"), "user/logout"); ?></li>
                </ul>
              </li>
              <li><?php echo l(t("logout"), "user/logout"); ?></li>
            <?php endif; ?>
            </ul>
          </div>
        </nav>
      </div>

    </div>
  </div>
</header>
<div class="master-container">
  <!-- Header -->
  <header role="banner" id="page-header">
    <?php print render($page['header']); ?>
  </header>
  <!-- Breadcrumbs -->
  <div id="breadcrumb-navbar">
    <div class="container">
      <div class="row">
        <div class="span19">
        <?php if ($breadcrumb): print $breadcrumb; endif;?>
        </div>
        <div class="span5 pull-right">
        <?php if ($search): ?>
          <?php if ($search): print render($search); endif; ?>
        <?php endif; ?>
        </div>
      </div>
    </div>
  </div>
  <!-- Title -->
  <?php if (drupal_is_front_page()): ?>
  <section class="page-header">
    <div class="container">
      <div class="row">
        <div class="span9">
          <div class="title">
            <?php if (theme_get_setting('welcome_message')): ?>
              <h1><?php print theme_get_setting('welcome_message'); ?></h1>
            <?php else: ?>
              <h1><span class="welcome">Welcome</span> to the <span class="site-name"><?php print $site_name ?></h1></span>
            <?php endif; ?>
           </div>
        </div>
      </div>
      <div class="page-header-content">
        <?php print render($page['homepage_header']); ?>
      </div>
    </div>
  </section>
  <?php else: ?>
    <section class="page-header">
        <div class="container">
          <div class="row">
            <span class="<?php print _apigee_base_content_span($columns); ?>">
              <!-- Title Prefix -->
              <?php print render($title_prefix); ?>
              <!-- Title -->
              <h1><?php print render($title); ?></h1>
              <!-- SubTitle -->
              <h2 class="subtitle"><?php print render($subtitle); ?></h2>              <!-- Title Suffix -->
              <?php print render($title_suffix); ?>
            </span>
          </div>
        </div>
    </section>
  <?php endif; ?>
  <div class="page-content">
    <div class="container">
      <?php print $messages; ?>
      <?php if ($page['help']): ?>
        <div class="well"><?php print render($page['help']); ?></div>
      <?php endif; ?>
      <?php if ($action_links): ?>
        <ul class="action-links"><?php print render($action_links); ?></ul>
      <?php endif; ?>
      <div class="row">
        <!-- Sidebar First (Left Sidebar)  -->
        <?php if ($page['sidebar_first']): ?>
          <aside class="span6 pull-left" role="complementary">
            <?php print render($page['sidebar_first']); ?>
          </aside>
        <?php endif; ?>
        <!-- Main Body  -->
        <section class="<?php print _apigee_base_content_span($columns); ?>">
          <?php if ($page['highlighted']): ?>
            <div class="highlighted hero-unit"><?php print render($page['highlighted']); ?></div>
          <?php endif; ?>
          <?php if (($tabs) && (!$is_front)): ?>
            <?php print render($tabs); ?>
          <?php endif; ?>
          <a id="main-content"></a>
          <?php print render($page['content']); ?>
        </section>
        <!-- Sidebar Second (Right Sidebar)  -->
        <?php if ($page['sidebar_second']): ?>
          <aside class="span6 pull-right" role="complementary">
            <?php print render($page['sidebar_second']); ?>
          </aside>  <!-- /#sidebar-second -->
        <?php endif; ?>
      </div>
    </div>
  </div>
</div>
<!-- Footer  -->
<footer class="footer">
  <div class="footer-inner">
    <div class="container">
      <?php print render($page['footer']); ?>
    </div>
  </div>
</footer>
