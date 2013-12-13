<?php

global $user;
?>
<div class="reports tabbable">
  <ul class="nav nav-tabs">
    <?php if (is_array($balances)): ?>
    <li <?php if ($active_tab == 'tab1') print 'class="active"'; ?> ><a data-toggle="tab" href="#tab1">Prepaid Balance</a></li>
    <li><a data-toggle="tab" href="#tab2">Reports</a></li>
    <?php else: ?>
    <li <?php if ($active_tab == 'tab2') print 'class="active"'; ?> ><a data-toggle="tab" href="#tab2">Reports</a></li>
    <?php endif; ?>
    <li <?php if ($active_tab == 'tab3') print 'class="active"'; ?> ><a data-toggle="tab" href="#tab3">Received Bills</a></li>
  </ul>
  <div class="tab-content">
    <?php if (is_array($balances)): ?>
    <div class="tab-pane<?php if ($active_tab == 'tab1') print ' active'; ?>" id="tab1">
      <h3>Current Prepaid Balance</h3>
      <table>
        <thead>
          <tr>
            <th>Account Currency</th>
            <th>Balance Brought Forward</th>
            <th>Top Ups</th>
            <th>Usage</th>
            <th>Tax</th>
            <th>Current Balance</th>
            <th>Actions</th>
          </tr>
        </thead>
        <tbody>
          <?php if ($has_balances): ?>
            <?php foreach ($balances as $balance): ?>
            <tr>
              <td><?php print $balance->supportedCurrency->name; ?></td>
              <td><?php print sprintf('%.2f', $balance->previousBalance);  ?></td>
              <td><?php print sprintf('%.2f', $balance->topups); ?></td>
              <td><?php print sprintf('%.2f', $balance->usage); ?></td>
              <td><?php print sprintf('%.2f', $balance->tax); ?></td>
              <td><?php print sprintf('%.2f', $balance->currentBalance); ?>&nbsp;&nbsp;
                <?php print l('Balance Detail (CSV)', 'users/me/monetization/billing/' . rawurlencode($balance->supportedCurrency->name) . '/' . rawurlencode(date('F-Y', time())), array('attributes' => array('style' => 'float:right'))); ?>
              </td>
              <td>
                <a href="javascript: topUpBalance('<?php print $balance->id; ?>', <?php print sprintf('%.2f', $balance->currentBalance); ?>, '<?php print $balance->supportedCurrency->name; ?>');" role="button" class="btn" >Top Up Balance</a>
              </td>
            </tr>
            <?php endforeach; ?>
          <?php endif; ?>
          <?php if ($can_top_up_another_currency): ?>
          <tr>
            <td>--</td>
            <td>--</td>
            <td>--</td>
            <td>--</td>
            <td>--</td>
            <td>--</td>
            <td><a href="javascript: topUpBalance();" role="button" class="btn" data-toggle="modal">Top Up Balance</a></td>
          </tr>
          <?php endif; ?>
        </tbody>
      </table>
      <div>
        <h3>Previous Prepaid Statements</h3>
        <?php print $form; ?>
      </div>
    </div>
    <?php endif; ?>
    <div class="tab-pane<?php if ($active_tab == 'tab2') print ' active'; ?>" id="tab2">
      <div style="margin-bottom: 20px;">
        <h3>Run Revenue Report</h3>
      </div>
      <div>
        <?php print $revenew_report_form; ?>
      </div>
    </div>
    <div class="tab-pane<?php if ($active_tab == 'tab3') print ' active'; ?>" id="tab3">
      <div class="row">
        <div class="span8">
          <?php print $billing_documents_form; ?>
        </div>
        <div class="span8 offset8">
          <form class="form-search">
            <input type="text" class="input-large search-query" placeholder="Search Billing Documents">
            <button type="submit" class="btn">Go</button>
          </form>
        </div>
      </div>
      <table>
        <thead>
          <th>Document Type</th>
          <th>Reference</th>
          <th>Products</th>
          <th>Received Date</th>
          <th>Download PDF</th>
        </thead>
        <tbody>
          <?php foreach ($billing_documents as $doc): ?>
          <tr>
            <td><?php print $doc['type'];  ?></td>
            <td><?php print $doc['ref']; ?></td>
            <td><?php print $doc['prods']; ?></td>
            <td><?php print $doc['rec_date'] ?></td>
            <td><?php print l(t('Download'), 'users/me/monetization/billing-document/' . rawurlencode($doc['ref']), array('attributes' => array('class' => array('btn')))); ?></td>
          </tr>
          <?php endforeach; ?>
        </tbody>
      </table>
    </div>
  </div>
</div>

<!-- Top Up Modal -->
<div id="topUp" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="topUpLabel" aria-hidden="true">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
    <h3 id="topUpLabel">Top Up Prepaid Balance <span id="currency_title"></span></h3>
    <div id="topup_alert" class="alert hide">
      <strong>Warning!</strong>&nbsp;The Amount to Top Up must be a valid number and bigger than zero.
    </div>
    <div id="currency_alert" class="alert hide">
      <strong>Warning!</strong>&nbsp;You must select currency.
    </div>
  </div>
  <div class="modal-body">
    <?php print $forms['top_up_form']; ?>
    <p>To top up your prepaid balance you will be taken to World Pay to process your payment.<br>
      Please enter the desired balance amount below.</p>
      <div id="currency_selector" style="margin-bottom: 10px; display:none;">
        <select>
          <option value="-1" selected="selected">select currency</option>
          <?php foreach ($currencies as $currency): ?>
          <option value="<?php print $currency->name; ?>"><?php print $currency->name . ' ('. $currency->displayName . ')'; ?></option>
          <?php endforeach; ?>
        </select>
      </div>
      <div style="margin-bottom: 10px;">
        <span class="topup-modal-label">Current Balance:</span>
        <span id="topUpCurrentBalance" class="topup-modal-value"></span>&nbsp;
        <span id="topUpCurrentBalanceCurrency" class="topup-modal-value"></span>
      </div>
      <div style="margin-bottom: 10px;">
        <span class="topup-modal-label">Amount to Top Up:</span>
        <span class="topup-modal-value">
          <input id="top-up-balance-input" type="text" placeholder="enter an amount" onkeyup="javascript: restrictRegexOnChangeEvent(this, /^[1-9][0-9]*((\.[0-9]{1,2})|\.)?$/, '#valid_top_up');">
          <input id="valid_top_up" type="hidden" />
        </span>
      </div>
      <div>
        <span class="topup-modal-label">New Balance:</span>
        <span id="newBalance" class="topup-modal-value"></span>&nbsp;
        <span id="newBalanceCurrency" class="topup-modal-value"></span>
      </div>
  </div>
  <div class="modal-footer">
    <a href="javascript: validateBalanceToTopUp();" class="btn btn-primary">Proceed to World Pay</a>
    <a class="btn" data-dismiss="modal" aria-hidden="true">Cancel</a>
  </div>
</div>
