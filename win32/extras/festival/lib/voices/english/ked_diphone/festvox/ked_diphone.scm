;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;                Centre for Speech Technology Research                  ;;
;;;                     University of Edinburgh, UK                       ;;
;;;                         Copyright (c) 1997                            ;;
;;;                        All Rights Reserved.                           ;;
;;;                                                                       ;;
;;;  Permission is hereby granted, free of charge, to use and distribute  ;;
;;;  this software and its documentation without restriction, including   ;;
;;;  without limitation the rights to use, copy, modify, merge, publish,  ;;
;;;  distribute, sublicense, and/or sell copies of this work, and to      ;;
;;;  permit persons to whom this work is furnished to do so, subject to   ;;
;;;  the following conditions:                                            ;;
;;;   1. The code must retain the above copyright notice, this list of    ;;
;;;      conditions and the following disclaimer.                         ;;
;;;   2. Any modifications must be clearly marked as such.                ;;
;;;   3. Original authors' names are not deleted.                         ;;
;;;   4. The authors' names are not used to endorse or promote products   ;;
;;;      derived from this software without specific prior written        ;;
;;;      permission.                                                      ;;
;;;                                                                       ;;
;;;  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK        ;;
;;;  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      ;;
;;;  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   ;;
;;;  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE     ;;
;;;  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    ;;
;;;  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   ;;
;;;  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          ;;
;;;  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       ;;
;;;  THIS SOFTWARE.                                                       ;;
;;;                                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  Set up kd_diphones using the standard diphone synthesizer
;;;
;;;  Kurt diphones: male American English collected Spring 1997
;;;

(defvar ked_diphone_dir (cdr (assoc 'ked_diphone voice-locations))
  "ked_diphone_dir
  The default directory for the ked diphone database.")
(set! load-path (cons (path-append ked_diphone_dir "festvox/") load-path))

(require 'radio_phones)
(require_module 'UniSyn)

;; set this to lpc or psola
(defvar ked_sigpr 'lpc)
;; Rset this to ungroup for ungrouped version
(defvar ked_groupungroup 'group)

(if (probe_file (path-append ked_diphone_dir "group/kedlpc16k.group"))
    (defvar ked_index_file 
      (path-append ked_diphone_dir "group/kedlpc16k.group"))
    (defvar ked_index_file 
      (path-append ked_diphone_dir "group/kedlpc8k.group")))

(set! ked_lpc_sep 
      (list
       '(name "ked_lpc_sep")
       (list 'index_file (path-append ked_diphone_dir "dic/diphdic_full.est"))
       '(grouped "false")
       (list 'coef_dir (path-append ked_diphone_dir "lpc"))
       (list 'sig_dir  (path-append ked_diphone_dir "lpc"))
       '(coef_ext ".lpc")
       '(sig_ext ".res")
       ))

(set! ked_lpc_group 
      (list
       '(name "ked_lpc_group")
       (list 'index_file ked_index_file)
       '(grouped "true")
       '(alternates_left ((ah ax)))
       '(alternates_right (($p p) ($k k) ($g g) ($d d) ($b b) ($t t)
				  (aor ao) (y ih) (ax ah) (ll l)))

       '(default_diphone "pau-pau")))

(cond
 ((and (eq ked_sigpr 'psola)
       (eq ked_groupungroup 'group))
  (set! ked_db_name (us_diphone_init ked_psola_group)))
 ((and (eq ked_sigpr 'psola)
       (eq ked_groupungroup 'ungroup))
  (set! ked_db_name (us_diphone_init ked_psola_sep)))
 ((and (eq ked_sigpr 'lpc)
       (eq ked_groupungroup 'group))
  (set! ked_db_name (us_diphone_init ked_lpc_group)))
 ((and (eq ked_sigpr 'lpc)
       (eq ked_groupungroup 'ungroup))
  (set! ked_db_name (us_diphone_init ked_lpc_sep))))

;;;;
;;;;  Our general diphone scheme allows identification of consonant
;;;   clusters etc the follow rules should work for American English
;;;;
(define (ked_diphone_const_clusters utt)
"(ked_diphone_const_clusters UTT)
Identify consonant clusters, dark ls etc in the segment stream
ready for diphone resynthesis.  This may be called as a post lexical
rule through poslex_rule_hooks."
  (mapcar
   (lambda (s) (ked_diphone_fix_phone_name utt s))
   (utt.relation.items utt 'Segment))
  utt)

(define (ked_diphone_fix_phone_name utt seg)
"(ked_diphone_fix_phone_name UTT SEG)
Add the feature diphone_phone_name to given segment with the appropriate
name for constructing a diphone.  Basically adds _ if either side is part
of the same consonant cluster, adds $ either side if in different
syllable for preceding/succeeding vowel syllable."
  (let ((name (item.name seg)))
    (cond
     ((string-equal name "pau") t)
     ((string-equal "-" (item.feat seg 'ph_vc))
      (if (and (member_string name '(r w y l))
	       (member_string (item.feat seg "p.name") '(p t k b d g))
	       (item.relation.prev seg "SylStructure"))
	  (item.set_feat seg "us_diphone_right" (format nil "_%s" name)))
      (if (and (member_string name '(w y l m n p t k))
	       (string-equal (item.feat seg "p.name") 's)
	       (item.relation.prev seg "SylStructure"))
	  (item.set_feat seg "us_diphone_right" (format nil "_%s" name)))
      (if (and (string-equal name 's)
	       (member_string (item.feat seg "n.name") '(w y l m n p t k))
	       (item.relation.next seg "SylStructure"))
	  (item.set_feat seg "us_diphone_left" (format nil "%s_" name)))
      (if (and (member_string name '(p t k b d g))
	       (member_string (item.feat seg "n.name") '(r w y l))
	       (item.relation.next seg "SylStructure"))
	  (item.set_feat seg "us_diphone_left" (format nil "%s_" name)))
      (if (and (member_string name '(p k b d g))
	       (string-equal "+" (item.feat seg 'p.ph_vc))
	       (not (member_string (item.feat seg "p.name") '(@ aa o)))
	       (not (item.relation.prev seg "SylStructure")))
	  (item.set_feat seg "us_diphone_right" (format nil "$%s" name)))
      (if (and (string-equal "l" name)
	       (string-equal "+" (item.feat seg "p.ph_vc"))
	       (not (string-equal "a" (item.feat seg "p.ph_vlng")))
	       (item.relation.prev seg 'SylStructure))
	  (item.set_feat seg "us_diphone_right" "ll"))
      (if (and (member_string name '(ch jh))
	       (string-equal "+" (item.feat seg 'p.ph_vc)))
	  (item.set_feat seg "us_diphone_right" "t"))
      )
   ((and (string-equal "ao" (item.name seg))
	 (string-equal "r" (item.feat seg 'n.name)))
    (item.set_feat seg "us_diphone_right" "aor"))
   ((string-equal "ah" (item.name seg))
    (item.set_feat seg "us_diphone" "ax"))
   ((string-equal "er" (item.name seg))
    ;; Cause the diphone have er-r for er we insert an r segment
    ;; This is a hack and leaves the utterance somewhat tainted (an
    ;; extra phone has appeared 
    (let ((newr (item.insert seg (list 'r) 'after)))
      (item.set_feat newr "end" (item.feat seg "end"))
      (item.set_feat seg "end" 
		     (/ (+ (item.feat seg "segment_start")
			   (item.feat seg "end"))
			2))))
   )))

;;;  Set up the CMU lexicon
(setup_cmu_lex)

(define (voice_ked_diphone)
"(voice_ked_diphone)
 Set up the current voice to be male  American English (Kurt) using
 the standard diphone corpus."
  ;; Phone set
  (voice_reset)
  (Parameter.set 'Language 'americanenglish)
  (require 'radio_phones)
  (Parameter.set 'PhoneSet 'radio)
  (PhoneSet.select 'radio)
  ;; Tokenization rules
  (set! token_to_words english_token_to_words)
  ;; POS tagger
  (require 'pos)
  (set! pos_lex_name "english_poslex")
  (set! pos_ngram_name 'english_pos_ngram)
  (set! pos_supported t)
  (set! guess_pos english_guess_pos)   ;; need this for accents
  ;; Lexicon selection
  (lex.select "cmu")
  (set! postlex_rules_hooks (list postlex_apos_s_check))
  ;; Phrase prediction
  (require 'phrase)
  (Parameter.set 'Phrase_Method 'prob_models)
  (set! phr_break_params english_phr_break_params)
  ;; Accent and tone prediction
  (require 'tobi)
  (set! int_tone_cart_tree f2b_int_tone_cart_tree)
  (set! int_accent_cart_tree f2b_int_accent_cart_tree)

  (set! postlex_vowel_reduce_cart_tree 
	postlex_vowel_reduce_cart_data)
  ;; F0 prediction
  (require 'f2bf0lr)
  (set! f0_lr_start f2b_f0_lr_start)
  (set! f0_lr_mid f2b_f0_lr_mid)
  (set! f0_lr_end f2b_f0_lr_end)
  (Parameter.set 'Int_Method Intonation_Tree)
  (set! int_lr_params
	'((target_f0_mean 105) (target_f0_std 15)
	  (model_f0_mean 170) (model_f0_std 34)))
  (Parameter.set 'Int_Target_Method Int_Targets_LR)
  ;; Duration prediction
  (require 'kddurtreeZ)
  (set! duration_cart_tree kd_duration_cart_tree)
  (set! duration_ph_info kd_durs)
  (Parameter.set 'Duration_Method Duration_Tree_ZScores)
  (Parameter.set 'Duration_Stretch 1.1)
  ;; Waveform synthesizer: ked diphones
  ;; This assigned the diphone names from their context (_ $ etc)
  (set! UniSyn_module_hooks (list ked_diphone_const_clusters ))
  (set! us_abs_offset 0.0)
  (set! window_factor 1.0)
  (set! us_rel_offset 0.0)
  (set! us_gain 0.9)

  (Parameter.set 'Synth_Method 'UniSyn)
  (Parameter.set 'us_sigpr ked_sigpr)
  (us_db_select ked_db_name)

  (set! current-voice 'ked_diphone)
)

(proclaim_voice
 'ked_diphone
 '((language english)
   (gender male)
   (dialect american)
   (description
    "This voice provides an American English male voice using a
     residual excited LPC diphone synthesis method.  It uses the
     CMU Lexicon for pronunciations.  Prosodic phrasing is provided 
     by a statistically trained model using part of speech and local 
     distribution of breaks.  Intonation is provided by a CART tree 
     predicting ToBI accents and an F0 contour generated from a model 
     trained from natural speech.  The duration model is also trained 
     from data using a CART tree.")))

(provide 'ked_diphone)
