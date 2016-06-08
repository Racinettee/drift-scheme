(println "Here's the sum of 1 2 3 and 4 + x from c++: " (+ 1 2 3 4 x))
(set! x (* 2 x))
(println "The new value of x: " x)
(define lam (lambda (z x)
	(print "I'm on the lam")
	(println ". I don't give a dam." " also the value of z: " z)
	(println "Also there's an x defined in this function: " x)))
(lam "eight six seven five three ohhhh niyyiiine" "Ramen Noodles")
(println "Is x still 200?? : " x)

(println "If you pass zero to the if statemnent you'll see blarg, else just arg: " (if 1 "arg" "blarg"))
(define megaman "MY name is megaman")
(println "Mega man's phrase twice: " (* megaman 2))
(set! megaman "Cutboy")
(println "Mega man changed his name to: " megaman)

(if 0
	(println "Single statement")
	(begin
		(println "This is the else statement")
		(println "It has multiple statements")))