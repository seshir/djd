<?php

namespace Apigee\ManagementAPI;

interface DeveloperInterface {

  public function load($email);
  public function validate($email = NULL);
  public function save($force_update = FALSE);
  public function delete($email = NULL);
  public function listDevelopers();
  public function validateUser();
  public function blankValues();

  /**
   * Populates the Developer object from a Drupal user object.
   *
   * This method should be removed from the KMS SDK and placed
   * inside a Drupal module instead.
   *
   * @deprecated
   *
   * @param \stdClass $account
   * @return void
   */
  public function populateFromUserAccount($account);

  public function getApps();
  public function getEmail();
  public function setEmail($email);
  public function getDeveloperId();
  public function getFirstName();
  public function setFirstName($fname);
  public function getLastName();
  public function setLastName($lname);
  public function getUserName();
  public function setUserName($uname);
  public function getStatus();
  public function setStatus($status);
  public function getAttribute($name);
  public function setAttribute($name, $value);
  public function getAttributes();
  public function getModifiedAt();
}