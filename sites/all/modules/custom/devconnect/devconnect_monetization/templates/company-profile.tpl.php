<?php

?>
<div class="tabbable">
  <ul class="nav nav-tabs">
    <li class="active"><a data-toggle="tab" href="#tab1">Contact Info</a></li>
    <li><a data-toggle="tab" href="#tab2">Bank &amp; Finance</a></li>
    <li><a data-toggle="tab" href="#tab3">Terms &amp; Conditions</a></li>
    <li><a data-toggle="tab" href="#tab4">Manage User Roles</a></li>
  </ul>
  <div class="tab-content">
    <div class="tab-pane company-info active" id="tab1">
      <?php print $company_profile_form; ?>
    </div>
    <div class="tab-pane bank-info" id="tab2">
      <div>Bank information is required in order to receive funds for Revenue Sharing plans.</div>
      <div style="float:left;">
        <?php print $company_bank_form; ?>
      </div>
<!--  <div style="float:right;">
        <h3>Credit Terms</h3>
      </div> -->
    </div>
    <div class="tab-pane tnc" id="tab3">
      <table>
        <thead>
          <tr>
            <th>Effective Date</th>
            <th>Terms &amp; Conditions</th>
            <th>T&amp;C Acceptance Date</th>
          </tr>
        </thead>
        <tbody>
          <?php foreach ($tncs_collection as $tnc): ?>
            <tr>
              <td><?php print $tnc['tnc']->getFormattedStartDate('M d Y'); ?></td>
              <td><a href="<?php print $tnc['tnc']->getUrl(); ?>"><?php print $tnc['tnc']->getUrl(); ?></a></td>
              <?php if (!isset($tnc['accepted'])): ?>
              <td><?php print $tnc['form']; ?></td>
              <?php else: ?>
              <td><?php print $tnc['accepted']; ?></td>
              <?php endif; ?>
            </tr>
          <?php endforeach; ?>
        </tbody>
      </table>
    </div>
    <div class="tab-pane user-roles" id="tab4">
      <div class="row">
        <div class="span11">
          <h3>Manage Users</h3>
          <?php print $manage_users_form; ?>
        </div>
        <div class="span11 offset2">
          <h3>Associate New User</h3>
          <?php print $associate_new_user_form; ?>
        </div>
      </div>
    </div>
  </div>
</div>