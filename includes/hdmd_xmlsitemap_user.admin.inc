<?php

function hdmd_xmlsitemap_user_admin_form($form, &$form_state) {
  // Add a class into the form.
  $form['#attributes']['class'] = 'hdmd-xmlsitemap-user-admin-form';

  // Attach css.
  $form['#attached']['css'] = array(drupal_get_path('module', 'hdmd_xmlsitemap_user') . '/theme/hdmd_xmlsitemap_user_admin_form.css');

  // Store the selected role id into a variable.
  $rid = isset($form_state['input']['filter']['role']) ? $form_state['input']['filter']['role'] : NULL;

  // Store the selected name id into a variable.
  $name = isset($form_state['input']['filter']['name']) ? $form_state['input']['filter']['name'] : NULL;

  // Store the selected mail id into a variable.
  $mail = isset($form_state['input']['filter']['mail']) ? $form_state['input']['filter']['mail'] : NULL;

  $status = isset($form_state['input']['filter']['status']) ? $form_state['input']['filter']['status'] : NULL;

  // Create a container for filter element.
  $form['filter'] = array(
    '#type'   => 'fieldset',
    '#title'  => t('Filter'),
    '#tree'   => TRUE
  );

  // Create a name textfield inside filter element.
  $form['filter']['name'] = array(
    '#type'   => 'textfield',
    '#title'  => t('Username')
  );

  // Create a mail textfield inside filter element.
  $form['filter']['mail'] = array(
    '#type'   => 'textfield',
    '#title'  => t('E-mail')
  );

  // Create a role select option inside filter element.
  $form['filter']['role'] = array(
    '#type'         => 'select',
    '#title'        => t('Role'),
    '#options'      => _get_available_user_roles(),
    '#empty_option' => t(' - Any -')
  );

  $form['filter']['status'] = array(
    '#type'         => 'select',
    '#title'        => t('Status'),
    '#options'      => array(
      'hdmd_xmlsitemap_user' => t('Added'),
      'not_added_yet' => t('Not yet added'),
    ),
    '#empty_option' => t('- Any -')
  );

  // Create a apply button inside filter element.
  $form['filter']['actions']['submit'] = array(
    '#type'   => 'submit',
    '#value'  => t('Apply'),
    '#submit' => array('hdmd_xmlsitemap_user_admin_form_filter_apply')
  );

  // Create a container for users element.
  $form['users'] = array(
    '#type'   => 'fieldset',
    '#title'  => t('Users'),
    '#weight' => 12
  );

  $header = hdmd_xmlsitemap_user_admin_form_header();
  $rows   = hdmd_xmlsitemap_user_admin_form_rows($name, $mail, $rid, $status);

  $form['users']['table'] = array(
    '#type'     => 'tableselect',
    '#header'   => $header,
    '#options'  => $rows,
    '#sticky'   => TRUE,
    '#empty'    => t('Nothing to display.')
  );

  $form['pager'] = array(
    '#markup' => theme('pager'),
    '#weight' => 11,
  );

  $form['submit'] = array(
    '#type'   => 'submit',
    '#name'   => 'config',
    '#value'  => t('Save configuration'),
    '#weight' => 10
  );


  return $form;
}

function hdmd_xmlsitemap_user_admin_form_filter_apply(&$form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

function hdmd_xmlsitemap_user_admin_form_submit(&$form, &$form_state) {
  //$rebuild = TRUE;

  if ($rows = array_filter($form_state['values']['table'])) {

    $sorted = array();

    $base_url = $GLOBALS['base_url'];

    foreach ($rows as $row) {
      $sorted[$row] = array(
        'type' => 'user',
        'subtype' => HDMD_XMLSITEMAP_USER_TYPE,
        'id' => $row,
        'loc' => $base_url . url('user/' . $row),
        'lastmod' => time(),
        'changefreq' => 0,
        'priority' => 0.5,
      );

      ksort($sorted);

      dpm($sorted);

      dpm(_testttt(8));

      $links = array_values($sorted);


      dpm($links);

      //_test_paaaa($links);

      /*foreach ($links as $link) {
        xmlsitemap_link_save($link);
      }*/
    }

    //$rebuild = FALSE;

    drupal_set_message(t('The configuration options have been saved.'));
  }

  $form_state['rebuild'] = TRUE;
}

function _get_user_xmlsitemap($uid) {
  $query = db_select('xmlsitemap', 'x')
          ->condition('x.subtype', HDMD_XMLSITEMAP_USER_TYPE)
          ->condition('x.id', $uid)
          ->fields('x', array());

  return $query->execute()->fetchAll();
}

function hdmd_xmlsitemap_user_xmlsitemap_links() {
  //_test_paaaa();
}

function _test_paaaa($links) {
  foreach ($links as $link) {
    xmlsitemap_link_save($link);
  }
}

function hdmd_xmlsitemap_user_admin_form_validate(&$form, &$form_state) {
  if ($form_state['clicked_button']['#name'] == 'config') {
    if (!array_filter($form_state['values']['table'])) {
      form_set_error('', t('Please select a value from the users table.'));
    }
  }
}

function hdmd_xmlsitemap_user_admin_form_header() {
  return array(
    'name'    => t('Username'),
    'mail'    => t('E-mail'),
    'role'    => t('Role'),
    'status'  => t('Status')
  );
}

function hdmd_xmlsitemap_user_admin_form_rows($name = NULL, $mail = NULL, $rid = NULL, $status = NULL) {
  $query = db_select('users_roles', 'ur')
          ->fields('ur', array('uid'))
          ->extend('PagerDefault')
          ->limit(HDMD_XMLSITEMAP_USER_ADMIN_ROWS_LIMIT);

  $query->join('users', 'u', 'u.uid = ur.uid');

  $query->fields('u', array('name', 'mail'));

  $query->join('role', 'r', 'r.rid = ur.rid');

  $query->addField('r', 'name', 'role');

  $query->leftJoin('xmlsitemap', 'x', 'x.id = u.uid');

  $query->addField('x', 'id', 'xid');

  //$query->extend('TableSort')->orderByHeader($header);

  if (!empty($name)) $query->condition('u.name', '%' . db_like($name) . '%', 'LIKE');

  if (!empty($mail)) $query->condition('u.mail', '%' . db_like($mail) . '%', 'LIKE');

  if (!empty($rid)) $query->condition('r.rid', $rid);

  if (!empty($status) && $status == HDMD_XMLSITEMAP_USER_TYPE) {
    $query->condition('x.subtype', HDMD_XMLSITEMAP_USER_TYPE);
  }

  if ($results = $query->execute()->fetchAll()) {
    foreach ($results as $user) {

      if ($status == 'not_added_yet' && !empty($user->xid)) {
        continue;
      }

      $rows[$user->uid]['name'] = $user->name;
      $rows[$user->uid]['mail'] = $user->mail;
      $rows[$user->uid]['role'] = $user->role;
      $rows[$user->uid]['status'] = !empty($user->xid) ? t('Added') : t('Not yet added');
    }
  }

  return isset($rows) ? $rows : array();
}

/**
 * @return
 *    $roles
 */
function _get_available_user_roles() {
  // Get the available user roles
  $roles = user_roles();

  // Remove anonymous from the option.
  if (isset($roles[1])) {
    unset($roles[1]);
  }

  // Remove authenticated from the option.
  if (isset($roles[2])) {
    unset($roles[2]);
  }

  return $roles;
}