/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Christopher Blizzard.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Christopher Blizzard <blizzard@mozilla.org>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

//changes copyright xiphos; license gpl2

#include "nsCOMPtr.h"
#include "dom/nsIDOMMouseEvent.h"

#include "dom/nsIDOMKeyEvent.h"
#include "dom/nsIDOMUIEvent.h"
#include "dom/nsIDOMNSEvent.h"
#include "nsIContent.h"

#include "EmbedEventListener.h"
#include "BrowserEmbed.h"
#include "geckowin/signals.h"
#include "main/sword.h"

EmbedEventListener::EmbedEventListener(void)
{
  mOwner = nsnull;
}

EmbedEventListener::~EmbedEventListener()
{
}

NS_INTERFACE_MAP_BEGIN(EmbedEventListener)
   NS_INTERFACE_MAP_ENTRY(nsIDOMEventListener)
NS_INTERFACE_MAP_END
NS_IMPL_ADDREF(EmbedEventListener)
NS_IMPL_RELEASE(EmbedEventListener)

nsresult
EmbedEventListener::Init(BrowserEmbed *aOwner)
{
  mOwner = aOwner;
  return NS_OK;
}

NS_IMETHODIMP
EmbedEventListener::HandleEvent(nsIDOMEvent* aDOMEvent)
{
	if (!aDOMEvent)
		return NS_OK;

	nsAutoString eventType;
	aDOMEvent->GetType(eventType);
	if (eventType.EqualsLiteral("mouseover"))
		MouseOver(aDOMEvent);
	if (eventType.EqualsLiteral("click"))
		MouseClick(aDOMEvent);
	if (eventType.EqualsLiteral("keydown"))
		KeyDown(aDOMEvent);
	if (eventType.EqualsLiteral("keyup"))
		KeyUp(aDOMEvent);
	if (eventType.EqualsLiteral("keypress"))
		KeyPress(aDOMEvent);
	if (eventType.EqualsLiteral("mousedown"))
		MouseDown(aDOMEvent);
	if (eventType.EqualsLiteral("mouseup"))
		MouseUp(aDOMEvent);
	if (eventType.EqualsLiteral("dblclick"))
		MouseDblClick(aDOMEvent);
	if (eventType.EqualsLiteral("mouseout"))
		MouseOut(aDOMEvent);


	return NS_OK;
}

nsresult
EmbedEventListener::KeyDown(nsIDOMEvent* aDOMEvent)
{
  nsCOMPtr <nsIDOMKeyEvent> keyEvent;
  keyEvent = do_QueryInterface(aDOMEvent);
  if (!keyEvent)
    return NS_OK;
  // Return FALSE to this function to mark the event as not
  // consumed...
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_KEY_DOWN], 0,
                (void *)keyEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}

nsresult
EmbedEventListener::KeyUp(nsIDOMEvent* aDOMEvent)
{
  nsCOMPtr <nsIDOMKeyEvent> keyEvent;
  keyEvent = do_QueryInterface(aDOMEvent);
  if (!keyEvent)
    return NS_OK;
  // return FALSE to this function to mark this event as not
  // consumed...
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_KEY_UP], 0,
                (void *)keyEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}

nsresult
EmbedEventListener::KeyPress(nsIDOMEvent* aDOMEvent)
{
  nsCOMPtr <nsIDOMKeyEvent> keyEvent;
  keyEvent = do_QueryInterface(aDOMEvent);
  if (!keyEvent)
    return NS_OK;
  // Return TRUE from your signal handler to mark the event as consumed.
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_KEY_PRESS], 0,
                (void *)keyEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}

nsresult
EmbedEventListener::MouseDown(nsIDOMEvent* aDOMEvent)
{
  nsCOMPtr <nsIDOMMouseEvent> mouseEvent;
  mouseEvent = do_QueryInterface(aDOMEvent);
  if (!mouseEvent)
    return NS_OK;
  // Return TRUE from your signal handler to mark the event as consumed.
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_MOUSE_DOWN], 0,
                (void *)mouseEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}

nsresult
EmbedEventListener::MouseUp(nsIDOMEvent* aDOMEvent)
{
  nsCOMPtr <nsIDOMMouseEvent> mouseEvent;
  mouseEvent = do_QueryInterface(aDOMEvent);
  if (!mouseEvent)
    return NS_OK;
  // Return TRUE from your signal handler to mark the event as consumed.
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_MOUSE_UP], 0,
                (void *)mouseEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}

nsresult
EmbedEventListener::MouseClick(nsIDOMEvent* aDOMEvent)
{
  nsCOMPtr <nsIDOMMouseEvent> mouseEvent;
  mouseEvent = do_QueryInterface(aDOMEvent);
  if (!mouseEvent)
    return NS_OK;
  // Return TRUE from your signal handler to mark the event as consumed.
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_MOUSE_CLICK], 0,
                (void *)mouseEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}

nsresult
EmbedEventListener::MouseDblClick(nsIDOMEvent* aDOMEvent)
{
	mOwner->ProcessMouseDblClickEvent(aDOMEvent);
	return NS_OK;
}

nsresult
EmbedEventListener::MouseOver(nsIDOMEvent* aDOMEvent)
{
	//GS_message(("mouse over"));
	mOwner->ChildFocusIn();
	nsCOMPtr <nsIDOMMouseEvent> mouseEvent;
	mouseEvent = do_QueryInterface(aDOMEvent);
	if (!mouseEvent)
		return NS_OK;
	// Return TRUE from your signal handler to mark the event as consumed.
	gint return_val = FALSE;
	g_signal_emit(G_OBJECT(mOwner->mOwner),
		      signals[HTML_DOM_MOUSE_OVER], 0,
		      (void *)mouseEvent, &return_val);
	if (return_val) {
		aDOMEvent->StopPropagation();
		aDOMEvent->PreventDefault();
	}
	return NS_OK;
}

nsresult
EmbedEventListener::MouseOut(nsIDOMEvent* aDOMEvent)
{
	mOwner->ChildFocusOut();
	nsCOMPtr <nsIDOMMouseEvent> mouseEvent;
  mouseEvent = do_QueryInterface(aDOMEvent);
  if (!mouseEvent)
    return NS_OK;
  // Return TRUE from your signal handler to mark the event as consumed.
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_MOUSE_OUT], 0,
                (void *)mouseEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}

nsresult
EmbedEventListener::Activate(nsIDOMEvent* aDOMEvent)
{
  nsCOMPtr <nsIDOMUIEvent> uiEvent = do_QueryInterface(aDOMEvent);
  if (!uiEvent)
    return NS_OK;
  // Return TRUE from your signal handler to mark the event as consumed.
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_ACTIVATE], 0,
                (void *)uiEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}

nsresult
EmbedEventListener::FocusIn(nsIDOMEvent* aDOMEvent)
{
  nsCOMPtr <nsIDOMUIEvent> uiEvent = do_QueryInterface(aDOMEvent);
  if (!uiEvent)
    return NS_OK;
  // Return TRUE from your signal handler to mark the event as consumed.
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_FOCUS_IN], 0,
                (void *)uiEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}

nsresult
EmbedEventListener::FocusOut(nsIDOMEvent* aDOMEvent)
{
  nsCOMPtr <nsIDOMUIEvent> uiEvent = do_QueryInterface(aDOMEvent);
  if (!uiEvent)
    return NS_OK;
  // Return TRUE from your signal handler to mark the event as consumed.
  gint return_val = FALSE;
  g_signal_emit(G_OBJECT(mOwner->mOwner),
                signals[HTML_DOM_FOCUS_OUT], 0,
                (void *)uiEvent, &return_val);
  if (return_val) {
    aDOMEvent->StopPropagation();
    aDOMEvent->PreventDefault();
  }
  return NS_OK;
}
