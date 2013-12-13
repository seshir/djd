<div class="api">
	<?php print $fields['field_product_logo']->content; ?><h3><?php print $fields['title']->content; ?></h3>
	<?php if(strip_tags($fields['field_upcoming']->content)) {?>
		<span class="label label-info"><?php print t('Upcoming'); ?></span>
	<?php } ?>
	<?php if(!strip_tags($fields['field_partner_access']->content)){?>
		<span class="label label-success"><?php print t('Public');?></span>
	<?php } else { ?>
		<span class="label label-important"><?php print l(t('Partner'),"api/partner/registration/".$fields['field_partner_access']->raw);?></span>
	<?php } ?>
	<p><?php print $fields['body']->content; ?></p>
</div>