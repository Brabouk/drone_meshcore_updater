/*
 * Advertising control for code that has to take the radio away from it.
 *
 * On a single-radio controller a legacy connectable advertiser and a scan
 * cannot be enabled at the same time: bt_le_scan_start() comes back -EINVAL
 * for as long as the advertiser is up, whatever the scan parameters are.
 * ble_scanner_with_radio_paused() solves the other direction (advertising
 * borrowing the radio back from a running scan); these two are that half's
 * mirror — the scanner takes the advertiser down, and the advertiser is put
 * back afterwards.
 *
 * The advertiser is the one that must yield, not the scan: a find has a
 * deadline and a target, while an unsolicited advertising restart can wait
 * the couple of seconds a scan window takes.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ADV_CONTROL_H_
#define ADV_CONTROL_H_

#include <stdbool.h>

/*
 * Stop advertising so the scanner can start. Returns 0 when the advertiser is
 * down — whether this call stopped it or it was already stopped — and a
 * negative errno if it could not be stopped, in which case nothing changed and
 * the caller should fail its scan rather than retry blindly.
 *
 * Not reentrant and not counted: the radio has one advertiser, so a second
 * pause while paused is a no-op and a resume always resumes.
 */
int adv_pause(void);

/* Put the advertiser back. Safe to call when it was never paused, and safe to
 * call twice — the resume path goes through the same retrying work item as a
 * disconnect does, because the start can still fail transiently. */
void adv_resume(void);

/* True while a central is connected to us (the browser's SMP session).
 *
 * A find that had to take the radio away from the advertiser must not take it
 * away from a live connection as well: scanning next to an active link costs
 * the link airtime it does not have to spare, and the peer that is connected
 * right now is the one a person is looking at. Callers wait instead.
 */
bool adv_session_busy(void);

#endif /* ADV_CONTROL_H_ */
