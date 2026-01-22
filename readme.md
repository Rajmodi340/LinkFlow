<!-- problem statement -->
design and implement a high performance url shortener service that can handle millions o requests wiht low latency and high availability


## functioncal requirement
#generate a short url form long url
#redirect long url to short url
#support expiry time for urls
#Implement rate limiting to prevent abuse

## non-functional requiremnt

low latency redirect (>50ms)
high availability
horizontal scalabaility
fault tolerance


## tech decisions
lnaguage :c++
focus on performance critical bacjend comaponents
in memory caching and sharding simulataion




## scope 
core url  shortening logoc
in memory storage
no http server yet



//how will i generate unique id
//counter based

# data handling
multiple short url for  same long?

# user Management
authenticatin require??

# lifecyllemanagement
how they expire ??
most manual but same limited time

# customisation
custom aliase
# security
how to prevent spam,phishing, mamlware


## the art of short code generation



# auto-incrementing ID with base 62 Encdoing
(counter(unique)+62(char) for mapping)


# hash-based generation

natural deduplication
two same hsort hash ocde due to 7 character taking
!! solution : character taking length badha do
** check url in database



## UUID-based genration
universally unique idnetifier
Bahut lamba

Practically unique hota hai

Do machines pe bhi generate karo → collision nahi



generate uuid
     
     uskko encode kro

     short code bnao


 ## database ##
 postgresql


 no sql might help in hgih scalbility time but relationalmodellwork very very well


    
## optimizing for read performance
with 100:1
read to write ratio
*multilayered cahching*
 
 redis: most frequently accessed short code mappings

 CDN: for exteremly short url cahce redirect the response
 this reduce latency

 **security and abuse prevention**

1.rate limiting
  to prevent form crash as same time 10000 url demand may crash db 

  2. malicius url detection
  

 maintain blocklist
 intergerate with google safe browsing
 regex check 

 3. enumeration attack protection



 4. expiry & auto cleanup
 old malicious url saty forever

 5. redirect safety



 **ensuring reliabilty and fault tolerance*

ek databse vahi dwn ho gya too :
      solution : master-slave replication
      master: likhne wala
      slave: padhne wala

      if master fail then slave become master
      system continues


**aplicaton resilience**

A) circuit breaker

agra koi service down ho to system he turn off
cascadin g failure se bachata h



b) gracefull degrdation

extra chz bnd ho jaye




c)  retry logic
kabhi kbahi network issue temporary




**global distribution**
a) geo DNS
user jha h uske nearest server pa le jao
 

 b) reigonal redis
 har reigon me apne redis

 c)edge computing
 redirect k logic server tk jaye



## CDN STRATEGY

We cache HTTP 302 redirect responses at the CDN edge using Cloudflare Workers, allowing popular short URLs to bypass the backend entirely and achieve sub-50ms global latency

## monitoring and observaility
checking system health
agar problema aye to kyu ayi


#key metrics


#eror rates
4xx->user glti
5xx->server galti

#business metrices
ye metrics engineerng + product dono


**alerting strategy**
jb koi metrices safe limit cross kre

we mointor latency percentiles,error rates and business metrics and configure alerts so issue are detected and resilved before impacting users

Track latency (P95)

Track error rates

Alert on anomalies

Logs for debugging