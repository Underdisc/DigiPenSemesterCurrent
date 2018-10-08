# Import needed functions.
library(plyr)
library(gdata)

# Read all of the data from the xls file.
xls_book <- read.xls("rollingsales_manhattan.xls", pattern="BOROUGH")
names(xls_book) <- tolower(names(xls_book))

# Create an empty data frame using the number of entries within the xls_book
# data frame. We will put all of the information we consider important for data
# analysis into this data frame.
entries <- nrow(xls_book)
data_id <- c(1:entries)
book_raw <- data.frame(id=data_id)

# Add data from the xls data frame to the empty data frame.
# Remove commas from specific data in order to treat it as numeric data.
book_raw$hood <- xls_book$neighborhood
book_raw$year_built <- as.numeric(xls_book$year.built)
book_raw$gross_sqft <- as.numeric(gsub("[^[:digit:]]", 
                                       "",
                                       xls_book$gross.square.feet))
book_raw$land_sqft <- as.numeric(gsub("[^[:digit:]]",
                                      "",
                                      xls_book$land.square.feet))
book_raw$sale_date <- as.Date(xls_book$sale.date)
book_raw$sale_price <- as.numeric(gsub("[^[:digit:]]",
                                       "",
                                       xls_book$sale.price))

# Clean up the book by removing sales with a 0 gross square footage or a 0 sale
# price.
book_cleanup_1 <- book_raw[book_raw$gross_sqft != 0,]
book <- book_cleanup_1[book_cleanup_1$sale_price > 100,]

# Compute the range, median, mean and standard deviation all of the sale price
# data in the data frame.
range_overall <- range(book$sale_price)
median_overall <- median(book$sale_price)
mean_overall <- mean(book$sale_price)
sd_overall <- sd(book$sale_price)

# Print out the calculated statistics for all of the sale price data.
print("Range of sale price for all data")
range_overall
print("Median of sale price for all data")
median_overall
print("Mean of sale price for all data")
mean_overall
print("Standard deviation of sale price for all data")
sd_overall

# Compute the range, median, mean and standard deviation for each neighborhood
# in the data frame. 
range_hood <- aggregate(book[, "sale_price"], list(book$hood), range)
median_hood <- aggregate(book[, "sale_price"], list(book$hood), median)
mean_hood <- aggregate(book[, "sale_price"], list(book$hood), mean)
sd_hood <- aggregate(book[, "sale_price"], list(book$hood), sd)

# Print out the calculated statistics for each neighborhood.
print("Range of sale price for each neighborhood")
range_hood
print("Median of sale price for each neighborhood")
median_hood
print("Mean of sale price for each neighborhood")
mean_hood
print("Standard deviation of sale price for each neighborhood")
sd_hood

# Create a plot of the average prices for each neighborhood.
png("neighborhood_mean_prices.png", width=600, height=600, units="px")
par(mar=c(15.0, 5.0, 5.0, 5.0))
barplot(mean_hood$x, 
        names.arg=mean_hood$Group.1, 
        main="Average Sale Price by Neighborhood",
        las=2)
dev.off()

# Create a plot of the median prices for each neighborhood.
png("neighborhood_median_prices.png", width=600, height=600, units="px")
par(mar=c(15.0, 5.0, 5.0, 5.0))
barplot(median_hood$x, 
        names.arg=median_hood$Group.1, 
        main="Median Sale Price by Neighborhood",
        las=2)
dev.off()

# Create a plot of the standard deviation of prices for each neighborhood.
png("neighborhood_sd_prices.png", width=600, height=600, units="px")
par(mar=c(15.0, 5.0, 5.0, 5.0))
barplot(sd_hood$x, 
        names.arg=sd_hood$Group.1, 
        main="Standard Deviation by Neighborhood",
        las=2)
dev.off()

# Create a plot of the log of sale prices versus time.
png("date_log_prices.png", width=600, height=600, units="px")
par(mar=c(15.0, 5.0, 5.0, 5.0))
plot(book$sale_date, 
     log(book$sale_price),
     xlab="Date",
     ylab="Log(Sale Price)",
     main="Log(Sale Price) vs Sale Date")
dev.off()

# Create a plot of the sale prices versus time.
png("date_prices.png", width=600, height=600, units="px")
par(mar=c(15.0, 5.0, 5.0, 5.0))
plot(book$sale_date, 
     book$sale_price,
     xlab="Date",
     ylab="Sale Price",
     main="Sale Price vs Sale Date")
dev.off()

# Create a plot of the sale prices versus time.
png("neighborhood_prices.png", width=600, height=600, units="px")
par(mar=c(15.0, 5.0, 5.0, 5.0))
plot(book$hood, 
     book$sale_price,
     xlab="Neighborhood",
     ylab="Sale Price",
     main="All Sale Prices For Each Neighborhood")
dev.off()


