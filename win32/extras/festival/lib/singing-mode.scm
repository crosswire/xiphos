;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Festival Singing Mode
;;;
;;; Written by Dominic Mazzoni
;;; Carnegie Mellon University
;;; 11-752 - "Speech: Phonetics, Prosody, Perception and Synthesis"
;;; Spring 2001
;;;
;;; This code is public domain; anyone may use it freely.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(require_module 'rxp)

(xml_register_id "-//SINGING//DTD SINGING mark up//EN"
		(path-append libdir "Singing.v0_1.dtd")
		)

(xml_register_id "-//SINGING//ENTITIES Added Latin 1 for SINGING//EN"
		 (path-append libdir  "sable-latin.ent")
		 )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; XML parsing functions
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;; singing_xml_targets
;;
;; This variable defines the actions that are to be taken when
;; parsing each of our XML tags: SINGING, PITCH, DURATION, and REST.
;;
;; When we get the pitch and duration of each token, we store them
;; in features of the token.  Later our intonation and duration
;; functions access these features.
;;

(defvar singing_xml_elements
'(
  ("(SINGING" (ATTLIST UTT)
   (set! singing_pitch_att_list nil)
   (set! singing_dur_att_list nil)
   (set! singing_global_time 0.0)
   (set! singing_bpm (get-bpm ATTLIST))
   (set! singing_bps (/ singing_bpm 60.0))
   nil
  )

  (")SINGING" (ATTLIST UTT)
    (xxml_synth UTT)  ;;  Synthesize the remaining tokens
    nil
  )

  ("(PITCH" (ATTLIST UTT)
   (set! singing_pitch_att_list ATTLIST)
   UTT)

  (")PITCH" (ATTLIST UTT)
   (let ((freq (get-freqs singing_pitch_att_list)))
	 (print "freqs")
	 (print freq)
	 (maptail (lambda (item) (item.set_feat item 'freq freq))
			  (utt.relation.items UTT 'Token)))
   UTT)

  ("(DURATION" (ATTLIST UTT)
   (set! singing_dur_att_list ATTLIST)
   UTT)

  (")DURATION" (ATTLIST UTT)
   (let ((dur (get-durs singing_dur_att_list)))
	 (print "durs")
	 (print dur)
	 (maptail (lambda (item) (item.set_feat item 'dur dur))
			  (utt.relation.items UTT 'Token)))
   UTT)

  ("(REST" (ATTLIST UTT)
   (let ((dur (get-durs ATTLIST)))
	 (print "rest durs")
	 (print dur)
	 (maptail (lambda (item) (item.set_feat item 'rest (car dur)))
			  (utt.relation.items UTT 'Token)))
   UTT)
))

;;
;; get-bpm
;;
;; Given the attribute list of a SINGING tag, returns the beats
;; per minute of the song from the BPM parameter.
;;

(define (get-bpm atts)
  (parse-number (car (car (cdr (assoc 'BPM atts))))))

;;
;; get-durs
;;
;; Given the attribute list of a DURATION tag, returns a list of
;; durations in seconds for the syllables of the word enclosed by
;; this tag.
;;
;; It first looks for a BEATS parameter, and converts these to
;; seconds using BPM, which was set in the SINGING tag.  If this
;; is not present, it looks for the SECONDS parameter.
;;

(define (get-durs atts)
  (set! seconds (car (car (cdr (assoc 'SECONDS atts)))))
  (set! beats (car (car (cdr (assoc 'BEATS atts)))))
  (if (equal? beats 'X)
	  (mapcar (lambda (x) (parse-number x))
			  (string2list seconds))
	  (mapcar (lambda (x) (/ (parse-number x) singing_bps))
			  (string2list beats))))

;;
;; get-freqs
;;
;; Given the attribute list of a PITCH tag, returns a list of
;; frequencies in Hertz for the syllables of the word enclosed by
;; this tag.
;;
;; It first looks for a NOTE parameter, which can contain a MIDI
;; note of the form "C4", "D#3", or "Ab6", and if this is not
;; present it looks for the FREQ parameter.
;;

(define (get-freqs atts)
  (set! freqs (car (car (cdr (assoc 'FREQ atts)))))
  (set! notes (car (car (cdr (assoc 'NOTE atts)))))
  (if (equal? notes 'X)
	  (mapcar (lambda (x) (parse-number x))
			  (string2list freqs))
	  (mapcar (lambda (x) (note2freq x))
			  (string2list notes))))

;;
;; note2freq
;;
;; Converts a string representing a MIDI note such as "C4" and
;; turns it into a frequency.  We use the convention that
;; A5=440 (some call this note A3).
;;

(define (note2freq note)
  (format t "note is %l\n" note)
  (set! note (print_string note))
  (set! l (string-length note))
  (set! octave (substring note (- l 2) 1))
  (set! notename (substring note 1 (- l 3)))
  (set! midinote (+ (* 12 (parse-number octave))
					(notename2midioffset notename)))
  (set! thefreq (midinote2freq midinote))
  (format t "note %s freq %f\n" note thefreq)
  thefreq)

;;
;; midinote2freq
;;
;; Converts a MIDI note number (1 - 127) into a frequency.  We use
;; the convention that 69 = "A5" =440 Hz.
;;

(define (midinote2freq midinote)
  (* 440.0 (pow 2.0 (/ (- midinote 69) 12))))

;;
;; notename2midioffset
;;
;; Utility function that looks up the name of a note like "F#" and
;; returns its offset from C.
;;

(define (notename2midioffset notename)
  (set! ans (parse-number (car (cdr (assoc_string notename note_names)))))
  ans
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Pitch modification functions
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;; singing_f0_targets
;;
;; This function replaces the normal intonation function used in
;; festival.  For each syllable, it extracts the frequency that
;; was calculated from the XML tags and stored in the token this
;; syllable comes from, and sets this frequency as both the start
;; and end f0 target.  Really straightforward!
;;

(define (singing_f0_targets utt syl)
  "(singing_f0_targets utt syl)"

  (let ((freq (parse-number (syl2freq syl)))
		(start (item.feat syl 'syllable_start))
		(end (item.feat syl 'syllable_end)))
	(list (list start freq)
		  (list end freq))))

;;
;; syl2freq
;;
;; Given a syllable, looks up the frequency in its token.  The token
;; stores a list of all of the frequencies associated with its
;; syllables, so this syllable grabs the frequency out of the list
;; corresponding to its index within the word.  (This assumes that
;; a frequency was given for each syllable, and that a token
;; corresponds directly to a word.  Singing-mode is not guaranteed
;; to work at all if either of these things are not true.)
;;

(define (syl2freq syl)
  (set! index (item.feat syl "R:Syllable.pos_in_word"))
  (set! freqs (item.feat syl "R:SylStructure.parent.R:Token.parent.freq"))
  (my_nth freqs index))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Duration modification functions
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;; singing_duration_method 
;;
;; Calculates the duration of each phone in the utterance, in three
;; passes.  Consult the three functions it calls, below, to see what
;; each one does.
;;

(define (singing_duration_method utt)
  (mapcar singing_adjcons_syllable (utt.relation.items utt 'Syllable))  
  (mapcar singing_do_syllable (utt.relation.items utt 'Syllable))
  (mapcar singing_fix_segment (utt.relation.items utt 'Segment))
  utt)

;;
;; singing_adjcons_syllable
;;
;; First pass.  Looks at the first phone of each syllable and
;; adjusts the starting time of this syllable such that the
;; perceived start time of the first phone is at the beginning
;; of the originally intended start time of the syllable.
;;
;; If this is not done, telling it to say the word "ta" at time
;; 2.0 actually doesn't "sound" like it says the "t" sound until
;; about 2.1 seconds.
;;
;; This function has a little bit of duplicated code from
;; singing_do_syllable, below - it could be modularized a little
;; better.
;;

(define (singing_adjcons_syllable syl)
  (set! conslen 0.0)
  (set! vowlen 0.0)
  (set! numphones 0)
  ;; sum up the length of all of the vowels and consonants in
  ;; this syllable
  (mapcar (lambda (s)
			(set! slen (get_avg_duration (item.feat s "name")))
			(set! numphones (+ 1 numphones))
			(if (equal? "+" (item.feat s "ph_vc"))
				(set! vowlen (+ vowlen slen))
				(set! conslen (+ conslen slen))))
		  (item.leafs (item.relation syl 'SylStructure)))
  (set! totlen (+ conslen vowlen))
  (set! syldur (syl2dur syl))
  ;; figure out the offset of the first phone
  (set! phone1 (item.daughter1 (item.relation syl 'SylStructure)))
  (set! offset (get_duration_offset (item.feat phone1 "name")))
 (if (< syldur totlen)
	  (set! offset (* offset (/ syldur totlen))))
  (set! prevsyl (item.prev (item.relation syl 'Syllable)))
  (format t "Want to adjust syl by %f\n" offset)
  (if (equal? nil prevsyl)
	  nil
	  (let ()
		(item.set_feat prevsyl 'subtractoffset offset)
		(item.set_feat syl 'addoffset offset))))

;;
;; singing_do_syllable
;;
;; Second pass.  For each syllable, adds up the amount of time
;; that would normally be spent in consonants and vowels, based
;; on the average durations of these phones.  Then, if the
;; intended length of this syllable is longer than this total,
;; stretch only the vowels; otherwise shrink all phones
;; proportionally.  This function actually sets the "end" time
;; of each phone using a global "singing_global_time" variable.
;;
;; We also handle rests at this point, which are tagged onto the
;; end of the previous token.
;;

(define (singing_do_syllable syl)
  (set! conslen 0.0)
  (set! vowlen 0.0)
  (set! numphones 0)
  ;; sum up the length of all of the vowels and consonants in
  ;; this syllable
  (mapcar (lambda (s)
			(set! slen (get_avg_duration (item.feat s "name")))
			(set! numphones (+ 1 numphones))
			(if (equal? "+" (item.feat s "ph_vc"))
				(set! vowlen (+ vowlen slen))
				(set! conslen (+ conslen slen))))
		  (item.leafs (item.relation syl 'SylStructure)))
  (set! totlen (+ conslen vowlen))
  (format t "Vowlen: %f conslen: %f totlen: %f\n" vowlen conslen totlen)
  (set! syldur (syl2dur syl))
  (set! addoffset (item.feat syl 'addoffset))
  (set! subtractoffset (item.feat syl 'subtractoffset))
  (set! offset (- subtractoffset addoffset))
  (if (< offset (/ syldur 2.0))
	  (let ()
		(set! syldur (- syldur offset))
		(format t "Offset: %f\n" offset)))
  (format t "Syldur: %f\n" syldur)
  (if (> totlen syldur)
	  ;; if the total length of the average durations in the syllable is
	  ;; greater than the total desired duration of the syllable, stretch
	  ;; the time proportionally for each phone
	  (let ((stretch (/ syldur totlen)))
		(mapcar (lambda (s)
				  (set! slen (get_avg_duration (item.feat s "name")))
				  (set! slen (* stretch slen))
				  (set! singing_global_time (+ slen singing_global_time))
				  (item.set_feat s 'end singing_global_time))
				(item.leafs (item.relation syl 'SylStructure))))

	  ;; otherwise, stretch the vowels and not the consonants
	  (let ((voweltime (- syldur conslen)))
		(let ((vowelstretch (/ voweltime vowlen)))
		  (mapcar (lambda (s)
					(set! slen (get_avg_duration (item.feat s "name")))
					(if (equal? "+" (item.feat s "ph_vc"))
						(set! slen (* vowelstretch slen)))
					(set! singing_global_time (+ slen singing_global_time))
					(item.set_feat s 'end singing_global_time))
				  (item.leafs (item.relation syl 'SylStructure))))))
  (set! restlen (car (syl2rest syl)))
  (format t "restlen %l\n" restlen)
  (if (> restlen 0)
	  (let (nil)
		(set! lastseg (item.daughtern (item.relation syl 'SylStructure)))
		(set! SIL (car (car (cdr (assoc 'silences (PhoneSet.description))))))
		(set! singing_global_time (+ restlen singing_global_time))
		(set! pau_item_desc (list SIL
								  (list
								   (list "end" singing_global_time))))
		(item.insert (item.relation lastseg 'Segment) pau_item_desc 'after))))

;;
;; singing_fix_segment
;;
;; Third pass.  Finds any segments (phones) that we didn't catch earlier
;; (say if they didn't belong to a syllable, like silence) and sets them
;; to zero duration
;;

(define (singing_fix_segment seg)
  (if (equal? 0.0 (item.feat seg 'end))
	  (if (equal? nil (item.prev seg))
		  (item.set_feat seg 'end 0.0)
		  (item.set_feat seg 'end (item.feat (item.prev seg) 'end)))
	  (format t "segment: %s end: %f\n" (item.name seg) (item.feat seg 'end))))
  
;; returns the duration of a syllable (stored in its token)
(define (syl2dur syl)
  (set! index (item.feat syl "R:Syllable.pos_in_word"))
  (set! durs (item.feat syl "R:SylStructure.parent.R:Token.parent.dur"))
  (my_nth durs index))

;; returns the duration of the rest following a syllable
(define (syl2rest syl)
  (set! index (item.feat syl "R:Syllable.pos_in_word"))
  (set! durs (item.feat syl "R:SylStructure.parent.R:Token.parent.dur"))
  (set! numsyllables (length durs))
  (set! pauselen (item.feat syl "R:SylStructure.parent.R:Token.parent.rest"))
  (if (equal? index (- numsyllables 1))
	  (if (equal? pauselen nil)
		  (list 0.0)
		  (list pauselen))
	  (list 0.0)))

;; get the average duration of a phone
(define (get_avg_duration phone)
  (let ((pd (assoc_string phone phoneme_durations)))
    (if pd
	(car (cdr pd))
	0.08)))

;; get the duration offset of a phone (see the description above)
(define (get_duration_offset phone)
  (parse-number (car (cdr (assoc_string phone phoneme_offsets)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Other utility functions
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; turns the string "0.5,1.0,2.5" into the list (0.5 1.0 2.5)
(define (string2list str)
  (if (string-matches str ".+,.+")
	  (cons (string-before str ",") (string2list (string-after str ",")))
	  (list (string-append str ""))))

;; applies fn to the last element in l"
(define (maptail fn l)
  (if (equal? l nil)
	  nil
	  (if (equal? (cdr l) nil)
		  (list (fn (car l)))
		  (cons (car l) (maptail fn (cdr l))))))


;; returns the nth element of list l, 0-based
(define (my_nth l n)
  (if (equal? n 0)
	  (car l)
	  (my_nth (cdr l) (- n 1))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Initializing and exiting singing mode
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;; singing_init_func
;;

(define (singing_init_func)
  "(singing_init_func) - Initialization for Singing mode"

  ;; use mwm's voice, it's very nice (or tll for female)
  (set! phoneme_durations  duration_ph_info)

  ;; need to use OGI's phoneme_durations since that's what mwm uses
;  (load (library_expand_filename "ogi_gswdurtreeZ_wb.scm"))

  ;; use our intonation function
  (Parameter.set 'Int_Method 'General)
  (Parameter.set 'Int_Target_Method Int_Targets_General)
  (set! int_general_params
		(list 
		 (list 'targ_func singing_f0_targets)))

  ;; use our duration function
  (Parameter.set 'Duration_Method singing_duration_method)

  ;; use our xml parsing function
  (set! singing_previous_elements xxml_elements)
  (set! xxml_elements singing_xml_elements))

;;
;; singing_exit_func
;;

(define (singing_exit_func)
  "(singing_exit_func) - Exit function for Singing mode"
  (set! xxml_elements singing_previous_elements))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Data tables
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(set! note_names
	  '((C 0)
		(C# 1)
		(Db 1)
		(D 2)
		(D# 3)
		(Eb 3)
		(E 4)
		(E# 5)
		(Fb 4)
		(F 5)
		(F# 6)
		(Gb 6)
		(G 7)
		(G# 8)
		(Ab 8)
		(A 9)
		(A# 10)
		(Bb 10)
		(B 11)
		(B# 12)
		(Cb 11)))


;;
;; The following list contains the offset into each phone that best
;; represents the perceptual onset of the phone.  This is important
;; to know to get durations right in singing.  For example, if the
;; offset for "t" is .060, and you want to start a "t" sound at
;; time 2.0 seconds, you should actually start the phone playing
;; at time 1.940 seconds in order for it to sound like the onset of
;; the "t" is really right at 2.0.
;;
;; These were derived empically by looking at and listening to the
;; waveforms of each phone for mwm's voice.
;;
(set! phoneme_offsets
'(
(t 0.050)
(T 0.050)
(d 0.090)
(D 0.090)
(p 0.080)
(b 0.080)
(k 0.090)
(g 0.100)
(9r 0.050) ;; r
(l 0.030)
(f 0.050)
(v 0.050)
(s 0.040)
(S 0.040)
(z 0.040)
(Z 0.040)
(n 0.040)
(N 0.040)
(m 0.040)
(j 0.090)
(E 0.0)
(> 0.0)
(>i 0.0)
(aI 0.0)
(& 0.0)
(3r 0.0)
(tS 0.0)
(oU 0.0)
(aU 0.0)
(A 0.0)
(ei 0.0)
(iU 0.0)
(U 0.0)
(@ 0.0)
(h 0.0)
(u 0.0)
(^ 0.0)
(I 0.0)
(dZ 0.0)
(i: 0.0)
(w 0.0)
(pau 0.0)
(brth 0.0)
(h# 0.0)
))


;;
;; Declare the new mode to Festival
;;

(set! tts_text_modes
   (cons
    (list
      'singing   ;; mode name
      (list         
       (list 'init_func singing_init_func)
       (list 'exit_func singing_exit_func)
       '(analysis_type xml)
       ))
    tts_text_modes))

(provide 'singing-mode)

